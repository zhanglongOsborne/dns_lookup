 #include <stdio.h>
 #include <stdlib.h>
 #include <assert.h>
 #include "dict.h"
 #include <string.h>


 //查找某个键的下标,找到了则返回-1
 static int  dict_find_index(dict *d,void *key); 
 static dict_entry *dict_find_entry(dict *d,void*key);
 static dict_entry *dict_add_entry(dict *d,void *key);
 static int  dict_if_need_expand(dict *d);
 static int  dict_expand(dict *d,int size);
 static int  next_power(int size);
 static void dict_table_init(hash_table *ht,int size);
 static void dict_table_clear(hash_table *ht);
 static int dict_rehash(dict *d,int n);

 static int dict_find_index_by_index(dict *d,char *buf,int start, int end);
 static dict_entry *dict_find_entry_by_index(dict *d,char *buff,int start,int end );
/*生成哈希值的函数*/
static unsigned int hash(const unsigned char *key); 
static unsigned int hash_by_str_index(const unsigned char *buf, int start, int end);


 static int strcmp_by_index(char *src,char *buf,int start ,int end);
 static unsigned int crc32( const unsigned char *buf);
 static unsigned int crc32_by_index(const unsigned char *buf,int start,int end);
 static unsigned int multipli(const unsigned char *buf);
 static unsigned int multipi_by_index(const unsigned char *buf,int start,int end);

 static const unsigned int  crc32tab[] = {
     0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
     0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
     0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
     0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
     0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
     0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
     0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
     0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
     0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
     0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
     0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
     0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
     0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
     0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
     0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
     0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
     0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
     0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
     0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
     0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
     0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
     0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
     0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
     0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
     0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
     0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
     0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
     0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
     0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
     0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
     0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
     0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
     0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
     0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
     0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
     0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
     0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
     0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
     0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
     0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
     0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
     0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
     0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
     0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
     0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
     0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
     0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
     0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
     0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
     0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
     0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
     0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
     0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
     0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
     0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
     0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
     0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
     0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
     0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
     0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
     0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
     0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
     0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
     0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL 
 };

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

static unsigned int crc32( const unsigned char *buf)
{
    unsigned int  crc;
    crc = 0xFFFFFFFF;
		for(int i=strlen(buf)-1;i>=0;i--){
        crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
		}
    return crc^0xFFFFFFFF;
}

static unsigned int crc32_by_index( const unsigned char *buf,int start,int end)
{
    unsigned int  crc;
    crc = 0xFFFFFFFF;
		for(int i=end;i>=start;i--){
        crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
		}
    return crc^0xFFFFFFFF;
}

static unsigned int multipli(const unsigned char *buf){
    unsigned int hash_key = 5381;
		for(int i=strlen(buf)-1;i>=0;i--){
        hash_key = ((hash_key << 5) + hash_key) + buf[i]; // hash * 33 + c 
		}
    return hash_key;

}
static unsigned int multipi_by_index(const unsigned char *buf,int start,int end){ //calculate the hash from end to start
    unsigned int hash_key = 5318;
    for(int i=end;i>=start;i--){
        hash_key = ((hash_key << 5) + hash_key) + buf[i];
    }
    return hash_key;
}

/*hash 函数*/
static unsigned int hash(const unsigned char *buf){
    return crc32(buf);
}

static unsigned int hash_by_str_index(const unsigned char *buf, int start, int end){
    return multipi_by_index(buf,start,end);
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
	   int h = hash(key); 
	   int i,index ;
	   dict_entry *de;
	   for(i=0; i<=1;i++){
	   		index = h & d->ht[i].size_mask;  //得到对应table的下标
				de = d->ht[i].table[index];
			/*遍历链表*/
			while(de){
				if( strcmp(de->key,key)==0){
					return -1;
				}
				de = de->next; 
			}	   	
			/*如果不是rehashing状态,ht[1]为NULL，直接退出*/
			if(!dict_is_rehashing(d)) break;	
	   }
	   return index;
}

static int dict_find_index_by_index(dict *d,char *buf,int start,int end){
	unsigned int h = hash_by_str_index(buf,start,end);
	dict_entry *de;
	int i,index;
	for(i=0;i<=1;i++){
		index = h & d->ht[i].size_mask;
		de = d->ht[i].table[index];
		while(de){
			if(strcmp_by_index(de->key,buf,start,end) == 0)
				return -1;
			de = de->next;
		}
		if(!dict_is_rehashing(d)) break;
	}
	return index;
}

static int strcmp_by_index(char *src,char *buf,int start ,int end){
	int src_len = strlen(src), i=start;
	char c;
	if(src_len != (end - start +1))
		return -1;
	while((c=*src++) !='\0' && i<=end){
		if(!(c == buf[i]))
			return -1;
		i++;
	}
	return 0;
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
    entry->val = NULL;
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

void *dict_find_by_index(dict *d,char *buff,int start, int end){
		if(dict_is_rehashing(d)) dict_rehash(d,1);
		dict_entry *entry = dict_find_entry_by_index(d,buff,start,end);
		return entry==NULL ? NULL : entry->val;
}

 
 
static dict_entry *dict_find_entry(dict *d,void*key){
    int h = hash(key); 
    int i,index ;
    dict_entry *de;
    for(i=0; i<=1;i++){
        index = h & d->ht[i].size_mask;  //得到对应table的下标
        de    = d->ht[i].table[index];
        /*遍历链表*/
        while(de){
            if( strcmp(de->key,key)==0){
                return de;
            }
            de = de->next; 
        }	   	
        /*如果不是rehashing状态,ht[1]为NULL，直接退出*/
        if(!dict_is_rehashing(d)) break;	
    }
    return NULL;
}

static dict_entry *dict_find_entry_by_index(dict *d,char *buff,int start,int end ){
  unsigned int h = hash_by_str_index(buff,start,end);
	dict_entry *de;
	int i,index;
	for(i=0;i<=1;i++){
		index = h & d->ht[i].size_mask;
		de = d->ht[i].table[index];
		while(de){
			if(strcmp_by_index(de->key,buff,start,end) == 0)
				return de;
			de = de->next;
		}
		if(!dict_is_rehashing(d)) break;
	}
	return NULL;
 
}

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
					//printf("rehash+++++++++ key is :%s\n",entry->key);
  				index      = hash(entry->key) & d->ht[1].size_mask; /*计算entry 在ht[1] 中的下标*/
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
 	 int hash_key = hash(key);
 	 int index;
 	 int i;
 	 dict_entry *entry,*prev_entry;
 	 for(i=0;i<=1;i++){
 	 	index = hash_key & d->ht[i].size_mask;
 	 	if(d->ht[i].table[index]!=NULL){
 	 		entry      = d->ht[i].table[index];
 	 		prev_entry = NULL;
 	 		while(entry){
 	 			if( strcmp(entry->key,key)==0){
 	 				if(prev_entry==NULL){ /*链表中的第一个元素*/
 	 					d->ht[i].table[index]  = entry->next;
 	 				}else{
 	 					prev_entry->next = entry->next;
 	 				}
	 	 				/*个数减一*/
	 	 			d->ht[i].used --;

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
            else
                break;
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

void dict_empty(dict *d){
	dict_iter *iter  = dict_get_iter(d);
	dict_entry *entry=NULL;
	while(1){
		entry = dict_iter_next_entry(iter);
		if(entry){
			free(entry);
		}else{
            if(d->ht[0].table != NULL)
			    free(d->ht[0].table);
			dict_table_clear(&d->ht[0]);
			if(dict_is_rehashing(d)&&d->ht[1].table !=NULL){
				free(d->ht[1].table);
				dict_table_clear(&d->ht[1]);	
			}
			break;
		}
	}
    if(iter != NULL){
        free(iter);
        iter = NULL;
    }
}
void dict_destory(dict *d){
	dict_empty(d);
	free(d);
  d = NULL;
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
		 			printf(",%s",(char *)entry->val);
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

int get_hash_table_used(dict *d){
    return d->ht[0].used>=d->ht[1].used?d->ht[0].used:d->ht[1].used;
}

