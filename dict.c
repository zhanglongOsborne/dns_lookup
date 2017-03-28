 #include <stdio.h>
 #include <stdlib.h>
 #include <assert.h>

 #include "dict.h"


 //查找某个键的下标,找到了则返回-1
 static int  dict_find_index(dict *d,void *key); 
 static dict_entry *dict_find_entry(dict *d,void*key);
 static dict_entry *dict_add_entry(dict *d,void *key);
 static int  dict_if_need_expand(dict *d);
 static int  dict_expand(dict *d,int size);
 static int  next_power(int size);
 static unsigned int dict_hash_unction(const unsigned char *buf, int len);
 static void dict_table_init(hash_table *ht,int size);
 static void dict_table_clear(hash_table *ht);
 static int dict_rehash(dict *d,int n);





 static int dict_can_resize =1;
 //默认开启rehash
 void dict_enable_resize(){
 	  dict_can_resize = 1;
 }
 //禁止rehash
 void dict_disable_resize(){
 	  dict_can_resize = 0;
 }
static int  next_power(int size){
 		int i=1;
 		while(1){
 			if(i>=size)
 				return i;
 			i *=2;
 		}
}
/*增加,查找,删除 都调用这个函数*/
static int  dict_if_need_expand(dict *d){
 		if(dict_is_rehashing(d)) return 0; //如果在rehashing则不需要扩容
 		hash_table *ht = &d->ht[0];
 		//自动rehash 1.rehash开关打开 2.ht->used >= ht->size
 		//强制rehash 1.rehash开关关闭 2.ht->used/ht->size > DICT_FORCE_REHASH_RATE
 		if( (dict_can_resize && ht->used >= ht->size) || (!dict_can_resize && ht->used/ht->size > DICT_FORCE_REHASH_RATE) ){
 			dict_expand(d,d->ht[0].used*2); // used * 2
 			return 1;
 		}else
 			return 0;
}


/*hash 函数*/
static unsigned int dict_hash_func(const unsigned char *buf){
    unsigned int hash = 5381;
    unsigned char c;
		printf("-------------the buf is:%s\n",buf);
    while ((c=*buf++)!='\0')
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

/*字典 函数属性初始化*/
static void dict_function_init(dict *d){
	dict_set_hash(d,dict_hash_func);           
    dict_set_key_match(d,NULL);         
    dict_set_key_dup(d,NULL);           
    dict_set_key_destory(d,NULL);      
    dict_set_val_match(d,NULL);        
    dict_set_val_dup(d,NULL);           
    dict_set_val_destory(d,NULL); 
}

/*初始化 hash-table*/
static void  dict_table_init(hash_table *ht,int size){
	  size          = next_power(size);
	  ht->table     = (dict_entry**)calloc(sizeof(dict_entry*),size);
	  ht->size      = size;
	  ht->size_mask = size -1;
	  ht->used      = 0;
}
/*清除 hash-table*/
static void dict_table_clear(hash_table *ht){
	  ht->table     = NULL;
	  ht->size      = 0;
	  ht->size_mask = 0;
	  ht->used      = 0;
}

dict *dict_init(){
	dict *d = (dict*)malloc(sizeof(dict));
    dict_function_init(d);
    dict_table_init(&d->ht[0],DICT_INIT_SIZE);  // new_power(DICT_INIT_SIZE) 确保table的大小为2^n
    dict_table_clear(&d->ht[1]);
    d->rehashindex = -1;
    return d;
}

//初始化 ht[1]
static int  dict_expand(dict *d,int size){
 	 dict_table_init(&d->ht[1],size);
	 d->rehashindex = 0;  /*start rehash*/
	 return 1;
}

static int  dict_find_index(dict *d,void *key){
	   int h = d->hash(key); 
	   int i,index ;
	   dict_entry *de;
	   for(i=0; i<=1;i++){
	   		index = h & d->ht[i].size_mask;  //得到对应table的下标
				de = d->ht[i].table[index];
			/*遍历链表*/
			while(de){
				if( (d->key_match && d->key_match(key,de->key)) || strcmp(de->key,key)==0){
					return -1;
				}
				de = de->next; 
			}	   	
			/*如果不是rehashing状态,ht[1]为NULL，直接退出*/
			if(!dict_is_rehashing(d)) break;	
	   }
	   return index;
}

//
int dict_add(dict *d,void *key,void *val){
	/*判断是否需要扩容*/
	dict_if_need_expand(d); 
	/*如果正在执行rehashing，则向前rehash一步*/
	if(dict_is_rehashing(d)) dict_rehash(d,1);  

	dict_entry *entry = dict_add_entry(d,key);
	if(entry!=NULL){
		entry->val = val;
		return 1;
	}else
		return 0;
}



static dict_entry *dict_add_entry(dict *d,void *key){
 		int index;
 		dict_entry *entry;
 		if((index=dict_find_index(d,key))==-1) /*如果对应的key已经存在*/
 			return NULL;
 		entry = (dict_entry*)malloc(sizeof(dict_entry));
 		entry->key = key;
 		/*如果在执行rehash操作,则添加到ht[1],否则添加到ht[0]*/
 		if(dict_is_rehashing(d)){
 			entry->next           = d->ht[1].table[index];
 			d->ht[1].table[index] = entry;
 			d->ht[1].used ++;
 		}else{
 			entry->next           = d->ht[0].table[index];
 			d->ht[0].table[index] = entry;
 			d->ht[0].used ++;
 		}
 		return entry;
 }

 void *dict_find(dict *d,void *key){
 	   /*如果正在执行rehashing，则向前rehash一步*/
	   if(dict_is_rehashing(d)) dict_rehash(d,1);  

 	   dict_entry *entry = dict_find_entry(d,key);
 	   return entry==NULL ? NULL : entry->val;
 }

 
 
static dict_entry *dict_find_entry(dict *d,void*key){
  	 int h = d->hash(key); 
	   int i,index ;
	   dict_entry *de;
	   for(i=0; i<=1;i++){
	   		index = h & d->ht[i].size_mask;  //得到对应table的下标
			de    = d->ht[i].table[index];
			/*遍历链表*/
			while(de){
				if( (d->key_match && d->key_match(key,de->key)) || strcmp(de->key,key)==0){
					return de;
				}
				de = de->next; 
			}	   	
			/*如果不是rehashing状态,ht[1]为NULL，直接退出*/
			if(!dict_is_rehashing(d)) break;	
	   }
	   return NULL;
  }

//替换
 int dict_replace(dict *d,void *key, void *val){
 	 dict_entry *entry = dict_find_entry(d,key);
 	 if(entry!=NULL)
 	 	entry->val = val;
	 return 1;
 }
  static int dict_rehash(dict *d,int n){
  		if(!dict_is_rehashing(d)) return 0;

  		int index;
  		dict_entry *entry;
  		dict_entry *next_entry;
  		while(n--){
  			

  			assert(d->rehashindex < d->ht[0].size);

  			while(d->ht[0].table[d->rehashindex]==NULL){
  				 d->rehashindex ++; /*跳过空的hash表*/
  				// if(d->rehashindex >= d->ht[0].size){  //保证下标是正确的避免段错误
  				// 	return 0;
  				// }
  			}
  			entry = d->ht[0].table[d->rehashindex];
  			while(entry){
  				next_entry = entry->next;
					printf("rehash+++++++++ key is :%s\n",entry->key);
  				index      = d->hash(entry->key) & d->ht[1].size_mask; /*计算entry 在ht[1] 中的下标*/
  				entry->next= d->ht[1].table[index];
  				d->ht[1].table[index] = entry;
  				entry                 = next_entry;
  				d->ht[1].used ++;
  				d->ht[0].used --;
  			}

  			/*刚好rehash完成*/
  			if(d->ht[0].used==0){
  				free(d->ht[0].table);  /*释放ht[0]的表空间*/
  				d->ht[0] = d->ht[1];
  				dict_table_clear(&d->ht[1]);
  				d->rehashindex = -1;  /*设置rehash的标志*/
  				return 0;
  			}

  			d->ht[0].table[d->rehashindex] = NULL; /*清空已rehash的entry*/
  			d->rehashindex ++ ;
  		}
  		return 1;
  }

 //删除
 int dict_delete(dict *d,void *key){
 	 int hash = d->hash(key);
 	 int index;
 	 int i;
 	 dict_entry *entry,*prev_entry;
 	 for(i=0;i<=1;i++){
 	 	index = hash & d->ht[i].size_mask;
 	 	if(d->ht[i].table[index]!=NULL){
 	 		entry      = d->ht[i].table[index];
 	 		prev_entry = NULL;
 	 		while(entry){
 	 			if( (d->key_match && d->key_match(entry->key,key)) || strcmp(entry->key,key)==0){
 	 				if(prev_entry==NULL){ /*链表中的第一个元素*/
 	 					d->ht[i].table[index]  = entry->next;
 	 				}else{
 	 					prev_entry->next = entry->next;
 	 				}
	 	 				/*个数减一*/
	 	 			d->ht[i].used --;

		 	 		if(d->key_destory!=NULL)
		 	 			d->key_destory(entry->key);
		 	 		if(d->val_destory!=NULL) 
		 	 			d->val_destory(entry->val);	
		 	 		free(entry);
		 	 		return 1;  /*删除完毕一个 key只有一个对应 entry*/
	 	 		}
 	 			prev_entry = entry;
 	 			entry = entry->next;
 	 		}
 	 	}	
 	 	/*如果没有rehash则,查找完ht[0]后直接退出*/
 	 	if(!dict_is_rehashing(d)) break;
 	 }
 	 return 1;
 }

  int dict_is_exist(dict *d, void *key){
  	  return dict_find_index(d,key)==-1 ? 1 : 0;
  }
 

static dict_entry *dict_iter_next_entry(dict_iter *iter){
	dict_entry *entry,*next_entry,*ret_entry;
	int index;
	int i=0;
	while(1){
		hash_table *ht=&iter->d->ht[iter->table];
		/*遍历链表*/
		if(iter->next_entry!=NULL){
			iter->entry = iter->next_entry;
			iter->next_entry = iter->entry->next;
			return iter->entry;
		}
		for(i=0;i<ht->size;i++){
			entry = ht->table[iter->index];
			if(entry==NULL) iter->index ++;
		}
		/*如果遍历完了*/
		if(iter->index >= ht->size){
			 /*如何处于rehash状态，且只遍历完ht[0]*/
			 if(dict_is_rehashing(iter->d) && iter->table==0){
			 	iter->table ++;
			 	iter->index =0;
			 	iter->entry = iter->next_entry = NULL;
			 	continue;
			 }else{
			 	return NULL; //遍历结束
			 }
		}

		iter->entry = entry;
		iter->next_entry = entry->next;
		iter->index ++ ;
		return iter->entry;
	}
}
 
 /*迭代器*/
dict_iter *dict_get_iter(dict *d){
	dict_iter *iter = (dict_iter*)malloc(sizeof(dict_iter));
	if(iter==NULL) return NULL;
	iter->d               = d;
	iter->index           = 0;
	iter->entry           = NULL;
	iter->next_entry      = NULL;
	iter->table           = 0;
	return iter; 
}
void *dict_iter_next(dict_iter *iter){
	dict_entry*entry;
	while(1){
		entry = dict_iter_next_entry(iter);
		if(entry!=NULL)
			return entry->val;
		else
			return NULL;
	}
}

dict_entry *dict_get_random_entry(dict *d){
	if((d->ht[0].used + d->ht[1].used)==0)  //空字典
		return NULL;
	if(dict_is_rehashing(d)) dict_rehash(d,1);
	int index ;
	dict_entry *entry,*p;
	if(dict_is_rehashing(d)){
		do{
			index = random()%(d->ht[0].size + d->ht[1].size);
			entry = (index >= d->ht[0].size) ? d->ht[1].table[index-d->ht[0].size] : d->ht[0].table[index];
		}while(entry==NULL);
	}else{
		do{
			index = random() % d->ht[0].size_mask;
			entry = d->ht[0].table[index];
		}while(entry==NULL);
	}
	int list_len=0;
	int n;
	p = entry;
	if(entry->next){
		while(entry){
			list_len ++;
			entry = entry->next;
		}
		n = random() % list_len;
		while(n--){
			p = p->next;
		}
	}
	return p;
}

void dict_empty(dict *d){
	dict_iter *iter  = dict_get_iter(d);
	dict_entry *entry=NULL;
	while(1){
		entry = dict_iter_next_entry(iter);
		if(entry){
			if(d->key_destory!=NULL) 
				d->key_destory(entry->key);
			if(d->val_destory!=NULL) 
				d->val_destory(entry->val);
			free(entry);
		}else{
			free(d->ht[0].table);
			dict_table_clear(&d->ht[0]);
			if(dict_is_rehashing(d)){
				free(d->ht[1].table);
				dict_table_clear(&d->ht[1]);	
			}
			break;
		}
	}
}
void dict_destory(dict *d){
	dict_empty(d);
	free(d);
}
void dict_display(dict *d){
	 int i;
	 int j;
	 dict_entry *entry;
	 for(i=0;i<=1;i++){
	 	printf("ht[%d] size=%d,used=%d\n",i,d->ht[i].size,d->ht[i].used);
	 	for(j=0;j<d->ht[i].size;j++){
	 		entry = d->ht[i].table[j];
	 		if(entry){
	 			printf("index=%d, ",j);
		 		while(entry){
		 			printf(",%s",entry->val);
		 			entry = entry->next;
		 		}
		 		printf("\n");
	 		}
	 	}
	 	if(!dict_is_rehashing(d)) break;
	 }
}
/*哈希表的状态 1.表大小 2.元素个数 3.slots的个数 4.元素最多的冲突链结点数 5.平均结点个数*/
 void dict_status(dict *d){
 	 int i;
	 int j;
	 int slots;
	 int max_chain_length;
	 float avg_chain_length; /*总的个数/slots*/
	 int tmp;
	 dict_entry *entry;
	 for(i=0;i<=1;i++){
	 	max_chain_length = 0;
	 	avg_chain_length = 0;
	 	slots            = 0;
	 	hash_table *ht= &d->ht[i];
	 	if(ht->used!=0){
		 	for(j=0;j<ht->size;j++){
		 		if(ht->table[j]!=NULL){
		 			slots ++;
		 			tmp  = 0;
		 			entry = ht->table[j];
		 			while(entry){
		 				tmp ++;
		 				entry = entry->next;
		 			}
		 			if(tmp>max_chain_length) max_chain_length = tmp;
		 		}
		 	}
		 	avg_chain_length = (float)ht->used /slots;
		 	printf("--------ht[%d] status--------\n",i);
		 	printf("table size        : %d\n",ht->size);
		 	printf("number of elements: %d\n",ht->used);
		 	printf("different slots   : %d\n",slots);
		 	printf("max_chain_length  : %d\n",max_chain_length);
		 	printf("avg_chain_length  : %0.02f\n\n",avg_chain_length);
	 	}else{
	 		printf("ht[%d] status size=%d,used=0\n",i,ht->size);
	 	}
	 	if(!dict_is_rehashing(d)) break;
	 }
 }



