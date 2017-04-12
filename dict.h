typedef struct dict_entry{
	void *key; 
	void *val;
	struct dict_entry *next; /*消除hash碰撞*/
	int key_len;
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
}dict;


/*迭代器*/
typedef struct dict_iter{
	dict *d;                   /*当前字典*/
	dict_entry *entry;       
	dict_entry *next_entry;
	int index;
	int table;
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

#define DICT_INIT_SIZE          1   //表空间初始大小

/*跟rehash相关*/
#define DICT_FORCE_REHASH_RATE  1                              //强制进行rehash操作, used / size 
#define dict_is_rehashing(d)    ((d)->rehashindex!=-1)    //dict是否处在rehashing状态



 dict *dict_init();

 //增加
 int dict_add(dict *d,void *key,int key_len,void *val);
 //查找
 void *dict_find(dict *d,void *key); 
 void *dict_find_by_index(dict *d,char *buff,int start, int end);

 //替换
 int dict_replace(dict *d,void *key, void *val);

 //删除
 int dict_delete(dict *d,void *key); 

 /*迭代器*/
dict_iter *dict_get_iter(dict *d);
void *dict_iter_next(dict_iter *iter);

 void dict_enable_resize();
 void dict_disable_resize();


 void dict_destory(dict *d);
 void dict_status(dict *d);

int get_hashtable_used(dict *d);









