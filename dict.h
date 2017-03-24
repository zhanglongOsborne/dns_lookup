typedef struct dict_entry{
	void *key; 
	void *val;
	struct dict_entry *next; /*消除hash碰撞*/
}dict_entry;



typedef struct hash_table{
	 dict_entry **table; /*哈希表空间*/ 
	 int size;           /*size=2^n*/
	 int size_mask;       /*sizemask = 2^n-1*/
	 int used;			 /*dict_entry的个数*/
}hash_table;

/*字典 1.支持rehash操作,2.支持多个不同模式的迭代器*/
typedef struct dict{
	hash_table ht[2];  /*两个哈希表,一般情况下只会用到ht[1],需要重新hash的时候才会用到ht[1]*/
	int  rehashindex;   /*rehash操作的下标,默认为-1,大于0表示正在进行rehash操作*/

	unsigned int (*hash)(const unsigned char *key);  /*生成哈希值的函数*/
	int (*hash_len)(const unsigned char *key,int len);  /*生成哈希值的函数*/
	/*key相关函数*/
	void *(*key_match)(void *key1,void*key2); /*比较*/
	void *(*key_dup)(void *key);              /*复制*/
	void (*key_destory)(void *key);         /*销毁*/

	/*值相关函数*/
	void *(*val_match)(void *val1,void*val2); /*比较*/
	void *(*val_dup)(void *val);              /*复制*/
	void (*val_destory)(void *val);         /*销毁*/
}dict;

/*迭代器*/
typedef struct dict_iter{
	int index;
	dict *d;                   /*当前字典*/
	int table;
	dict_entry *entry;       
	dict_entry *next_entry;
}dict_iter;




/**
 *  功能需求
 *
 *  1.增加 
 *  	1.如果dict处于rehash状态则添加到新表
 *  2.查找
 *  	1.默认表1开始,如果处于rehash状态需要到表2中找
 *  3.替换
 *  4.rehash
 *  	1.自动rehash  键值对的数目大于表空间的大小
 *  	2.强制rehash  键值对的数目/ 表空间的大小 大于某个某个特定的比例
 *  5.迭代
 */

#define DICT_INIT_SIZE          8   //表空间初始大小

/*跟rehash相关*/
#define DICT_FORCE_REHASH_RATE  1.5                              //强制进行rehash操作, used / size 
#define dict_is_rehashing(d)    ((d)->rehashindex!=-1)    //dict是否处在rehashing状态

/*设置值*/
#define dict_entry_set_val(entry,val)      ((entry)->val    = (val))

#define dict_set_hash(d,func)              ((d)->hash        = func)
#define dict_set_key_match(d,func)         ((d)->key_match   = func)
#define dict_set_key_dup(d,func)           ((d)->key_dup = func)
#define dict_set_key_destory(d,func)      ((d)->key_destory = func)

#define dict_set_val_match(d,func)         ((d)->val_match = func)
#define dict_set_val_dup(d,func)           ((d)->val_dup = func)
#define dict_set_val_destory(d,func)      ((d)->val_destory = func)



dict *dict_init();
 //增加
 int dict_add(dict *d,void *key,void *val);
 //查找
 void *dict_find(dict *d,void *key); 

 //替换
 int dict_replace(dict *d,void *key, void *val);

 //判断某个key是否存在
 int dict_is_exist(dict *d, void *key);

 //删除
 int dict_delete(dict *d,void *key); 

 /*迭代器*/
dict_iter *dict_get_iter(dict *d);
void *dict_iter_next(dict_iter *iter);

//随机获取一个entry
dict_entry *dict_get_random_entry(dict *d);

 void dict_enable_resize();
 void dict_disable_resize();


 void dict_destory(dict *d);
 void dict_status(dict *d);











