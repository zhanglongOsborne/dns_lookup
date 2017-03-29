#include<stdio.h>
#include"dns_lookup.h"
#include<string.h>

tree_node* init_tree_node(tree_node *new_node,char*name,int level){
	//new_node = (tree_node*)malloc(sizeof(tree_node));
	new_node->node_name = (char*)malloc(strlen(name)+1);
	strcpy(new_node->node_name,name);
	new_node->dict = NULL;
	new_node->father = NULL;
	new_node->brother = NULL;
	new_node->first_child = NULL;
	new_node->last_child = NULL;
	new_node->child_num = 0;
	new_node->level = level;
	return new_node;
}

//销毁树节点的时候需要销毁该节点维护的dict，并且从父节点dict中删除该节点
boolean destory_tree_node(tree_node* node){
	if(node != NULL){
		if(node->first_child !=NULL || node->last_child != NULL){
			printf("the node you destoryed has child node !");
			return false;
		}
		//删除该节点维护的dict
		if(node->dict !=NULL){
			dict_destory(node->dict);
		}
		//从父节点dict中hash_table中删除该节点
		if(node->father != NULL){
			dict_delete(node->father->dict,node->node_name);
			node->father->child_num--;	
		}
		node->father = NULL;
		node->first_child = NULL;
		node->last_child = NULL;
        
		free(node->node_name);
		node->node_name = NULL;
      	free(node);
	    node = NULL;
	}
	return true;
}

//删除的时候只能使用后根删除的方法
void delete_tree_node(tree_node *dele_node){
	if(dele_node->first_child != NULL){
		while(dele_node->first_child != NULL){
			tree_node *tmp = dele_node->first_child;
			dele_node->first_child = dele_node->first_child->brother;
            if(dele_node->first_child == NULL)
                dele_node->last_child = NULL;
			delete_tree_node(tmp);
		}
	}	

	destory_tree_node(dele_node);
    dele_node = NULL;
}
//插入节点的时候需要向父节点的dict中添加该子节点
boolean add_tree_node(tree_node *father,tree_node *new_node){
	if(find_tree_node(father,new_node->node_name)!=NULL){
		//printf("the node you added is already exist!\n");
		return false;
	}
	if(father->last_child != NULL){
		father->last_child->brother = new_node;
		father->last_child = new_node;
	}else{//当父节点没有子节点的时候，这时候加入子节点就需要开辟一个dict
		father->first_child = new_node;
		father->last_child = new_node;
		father->dict = dict_init();
	}
	if(!dict_add(father->dict,new_node->node_name,new_node))
		return false;

	father->child_num ++;
	new_node->father = father;
	new_node->level = father->level+1;
	return true;
}

boolean add_dns(tree_node *root,char *dns){
	if(strict_find_dns(root,dns)!=NULL){
		//printf("the dns rule you want to add is already exist :%s\n",dns);
		return true;
	}
	tree_node *current = NULL;
	if((current = suffix_find_dns(root,dns)) == NULL){
		current = root;
	}
	int current_level = current->level;//root看做是第0层
	IString array;
	split(dns,".",&array);
	for(int i=array.num-current_level-1;i>=0;i--){
		tree_node *new_node = (tree_node*)malloc(sizeof(tree_node));		
		init_tree_node(new_node,array.str[i],current->level+1);	
		if(!add_tree_node(current,new_node))
			continue;
		current = new_node;
	}
	free_IString(&array);
	return true;
	
}

tree_node* find_tree_node(tree_node *father,char *node_name){
	if(father->dict == NULL)
		return NULL;
	return dict_find(father->dict,node_name);
}
tree_node* strict_find_dns(tree_node *root,char *dns){
	IString array;//使用完后需要释放内存
	split(dns,".",&array);
	tree_node *current = root;
	for(int i=array.num-1;i>=0;i--){
		if(current->first_child == NULL)
			return NULL;
		if((current = dict_find(current->dict,array.str[i])) == NULL)
			return NULL;
	}
	free_IString(&array);
	return current;
}

tree_node* suffix_find_dns(tree_node *root,char *dns){
	IString array;
	split(dns,".",&array);
	tree_node *current = root;
	for(int i=array.num-1;i>=0;i--){
		if(current->first_child == NULL)
			return current;
		if((current = dict_find(current->dict,array.str[i])) == NULL)
			return NULL;
	}
	free_IString(&array);
	return current;
}

boolean delete_dns(tree_node *root,char *dns){
	tree_node *first = strict_find_dns(root,dns);
	if(first == NULL){
		printf("the dns you want to delete is not exist: %s\n",dns);
		return false;
	}
	//如果存在儿子节点，则不删除
	if(first->first_child != NULL)
		return true;
	while(first->brother == NULL && first != root) first = first->father;
	//注意，这里删除的时候先将查找到的first节点从父节点dict中删除。		
	if(first == root ){
		delete_tree_node(first->first_child);
		return true;
	}
	dict_delete(first->father->dict,first->node_name);
	delete_tree_node(first);
	return true;
}



int split(char *src, char *delim, IString* istr)
{
	int i;
	char *str = NULL, *p = NULL;
	(*istr).num = 1;
	str = (char*)calloc(strlen(src)+1,sizeof(char));
	if (str == NULL) return 0;
	(*istr).str = (char**)calloc(1,sizeof(char *));
	if ((*istr).str == NULL) return 0;
	strcpy(str,src);

	p = strtok(str, delim);
	(*istr).str[0] = (char*)calloc(strlen(p)+1,sizeof(char));
	if ((*istr).str[0] == NULL) return 0;
	strcpy((*istr).str[0],p);
	for(i=1; p = strtok(NULL, delim); i++)
	{
		(*istr).num++;
		(*istr).str = (char**)realloc((*istr).str,(sizeof((*istr).str)+1)*sizeof(char *));
		if ((*istr).str == NULL) return 0;
		(*istr).str[i] = (char*)calloc(strlen(p)+1,sizeof(char));
		if ((*istr).str[0] == NULL) return 0;
		strcpy((*istr).str[i],p);
	}
	free(str);
	str = p = NULL;

	return 1;
}

void free_IString(IString *istring){
	for(int i=0;i<istring->num;i++){
		free(istring->str[i]);
		istring->str[i] = NULL;
	}	
    if(istring->str != NULL){
        free(istring->str);
		istring->str = NULL;
	}
}


