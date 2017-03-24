#include"dict.h"

#define true 1
#define false 0

typedef int boolean;

typedef struct tree_node{
	char *node_name;
	dict *dict;
	int level;
	struct tree_node *father;
	struct tree_node *brother;
	struct tree_node *first_child;
	struct tree_node *last_child;
	int child_num;
}tree_node;

typedef struct {
	char **str;     //the PChar of string array
	int num;     //the number of string
}IString;


tree_node* init_tree_node(tree_node *new_node,char*name,dict*dic,int level,tree_node *father,tree_node *first_child,tree_node *last_child);

boolean destory_tree_node(tree_node *node);

void delete_tree_node(tree_node *dele_node);

boolean add_tree_node(tree_node *father,tree_node *new_node);

tree_node* find_tree_node(tree_node *father,char* node_name);

boolean delete_dns(tree_node *root,char *dns);

boolean add_dns(tree_node *root,char *dns);

tree_node* strict_find_dns(tree_node *root,char* dns);

tree_node* suffix_find_dns(tree_node *root,char* dns);


void split(char *s,char *delim,char **ret_list);
void free_IString(IString *istring);
