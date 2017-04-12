#include"dict.h"

#define true 1
#define false 0

typedef int boolean;

typedef struct tree_node{
	char *node_name;
	dict *dict;
	struct tree_node *father;
	int name_len;
	int level;
	int child_num;
	boolean terminal_flag;
}tree_node;

typedef struct {
	char **str;     //the PChar of string array
	int num;     //the number of string
}IString;


tree_node* init_tree_node(tree_node *new_node,char*buff,int start,int end,int level);

boolean destory_tree_node(tree_node *node);

void delete_tree_node(tree_node *dele_node);

boolean add_tree_node(tree_node *father,tree_node *new_node);

tree_node* find_tree_node(tree_node *father,char* dns,int start,int end);

boolean delete_dns(tree_node *root,char *dns,int dns_len,char delim);

boolean add_dns(tree_node *root,char *dns,int dns_len,char delim);

tree_node* strict_find_dns(tree_node *root,char* dns,int dns_len,char delim);

tree_node* suffix_find_dns(tree_node *root,char* dns,int dns_len ,char delim);

tree_node *strict_suffix_find_dns(tree_node *root,char *dns,int dns_len,char delim);


