#include<stdio.h>
#include<stdlib.h>
#include"dns_lookup.h"
#include<string.h>

int find_the_start_index(char *dns,int end,char delim){
	if(end < 0)
		return -1;
	for(int i=end;i>=0;i--){
		if(dns[i] == delim)
			return i+1;
	}
	return 0;
}

tree_node* init_tree_node(tree_node *new_node,char*buff,int start,int end,int level){
	new_node->node_name = (char*)malloc(end-start+2);
  memset(new_node->node_name,0,end-start+2);
	strncat(new_node->node_name,buff+start,end-start+1);
	new_node->dict = NULL;
	new_node->father = NULL;
	new_node->name_len = end-start+1;
	new_node->child_num = 0;
	new_node->level = level;
	new_node->terminal_flag = false;
	return new_node;
}



//销毁树节点的时候需要销毁该节点维护的dict，并且从父节点dict中删除该节点
boolean destory_tree_node(tree_node* node){
	if(node != NULL){
		if(node->child_num > 0){
			printf("the node you destoryed has child node !");
			return false;
		}
		//删除该节点维护的dict
		if(node->dict !=NULL){
			dict_destory(node->dict);
            node->dict = NULL;
		}
		//从父节点dict中hash_table中删除该节点
		if(node->father != NULL){
			dict_delete(node->father->dict,node->node_name);
			node->father->child_num--;	
		}
		node->father = NULL;
		free(node->node_name);
		node->node_name = NULL;
    free(node);
	  node = NULL;
	}
	return true;
}

//删除的时候只能使用后根删除的方法
void delete_tree_node(tree_node *dele_node){
	if(dele_node->child_num > 0){
        dict_iter *iter = dict_get_iter(dele_node->dict);
        tree_node *tmp = NULL;
		while((tmp = dict_iter_next(iter)) != NULL){
			delete_tree_node(tmp);
		}
        if(iter != NULL){
            free(iter);
            iter = NULL;
        }
	}	
	destory_tree_node(dele_node);
  dele_node = NULL;
}

//插入节点的时候需要向父节点的dict中添加该子节点
boolean add_tree_node(tree_node *father,tree_node *new_node){
	if(find_tree_node(father,new_node->node_name,0,strlen(new_node->node_name)-1)!=NULL){
		//printf("the node you added is already exist!\n");
		return false;
	}
  if(father->dict == NULL)
    father->dict = dict_init();
	if(!dict_add(father->dict,new_node->node_name,new_node->name_len,new_node))
		return false;

	father->child_num ++;
	new_node->father = father;
	new_node->level = father->level+1;
	return true;
}


boolean add_dns(tree_node *root,char *dns,int dns_len,char delim){
	//当存在该dns的时候需要将最后一级域名的terminal_flag 置1
	tree_node *current = root;
	int end = dns_len-1;
	int start = end;
	tree_node *tmp = root;
	while(end >= 0 && tmp != NULL){
		start = find_the_start_index(dns,end,delim);
		tmp = find_tree_node(tmp,dns,start,end);
		if(tmp != NULL){
			current = tmp;
			end = start - 2;
		}
	}
	if(end<0){    //说明插入的dns在树中
		current->terminal_flag = true;
		return true;
	}	
	
	//构造要插入的节点链表，等构造完成了在将头节点连接到current中
	tree_node *head = (tree_node*)malloc(sizeof(tree_node));
	memset(head,0,sizeof(tree_node));
	start = find_the_start_index(dns,end,delim);
	init_tree_node(head,dns,start,end,current->level+1);
	end = start - 2;

	//构造孩子链表
	tmp = head;
	while(end >= 0){
		start = find_the_start_index(dns,end,delim);
		tree_node *new_node = (tree_node*)malloc(sizeof(tree_node));
		memset(new_node,0,sizeof(tree_node));
		init_tree_node(new_node,dns,start,end,tmp->level+1);
		add_tree_node(tmp,new_node);
		tmp = new_node;
		end = start - 2;
	}
	tmp->terminal_flag = true;

	//构造完了将head加入到current中
	return add_tree_node(current,head);
}

tree_node* find_tree_node(tree_node *father,char *dns,int start,int end){
	if(father->dict == NULL)
		return NULL;
	return dict_find_by_index(father->dict,dns,start,end);
}

tree_node* strict_find_dns(tree_node *root,char *dns,int dns_len,char delim){
  tree_node *current = root;
	int end = dns_len - 1;
	int start = end;
	while(end >= 0){
		start = find_the_start_index(dns,end,delim);
		if(current->child_num == 0 || (current = find_tree_node(current,dns,start,end)) == NULL)
			return NULL;
		end = start - 2;
	}
	return current->terminal_flag == true ? current : NULL;
    
}

tree_node* suffix_find_dns(tree_node *root,char *dns,int dns_len,char delim){
	tree_node *current = root;
	tree_node *result = current;
	int end = dns_len - 1;
	int start = end;
	while(end >= 0){
		start = find_the_start_index(dns,end,delim);
		if(current->child_num == 0 && current->terminal_flag)
			return current;
		if((current = find_tree_node(current,dns,start,end)) == NULL)
			break;
		if(current->terminal_flag)
			result = current;
		end = start - 2;
	}
	return result->terminal_flag == true ? result : NULL;
}

tree_node* strict_suffix_find_dns(tree_node *root,char *dns,int dns_len,char delim){
	tree_node *current = root;
	tree_node *result = current;
	int end = dns_len - 1;
	int start = end;
	while(end >= 0){
		start = find_the_start_index(dns,end,delim);
		if(current->child_num == 0 && current->terminal_flag)
			return current;
		if((current = find_tree_node(current,dns,start,end)) == NULL)
			break;
		if(current->terminal_flag && start > 0)
			result = current;
		end = start - 2;
	}
	return result->terminal_flag == true ? result : NULL;
}

boolean delete_dns(tree_node *root,char *dns,int dns_len,char delim){
	tree_node *first = strict_find_dns(root,dns,dns_len,delim);
	if(first == NULL){
		printf("the dns you want to delete is not exist: %s\n",dns);
		return false;
	}
	//如果存在儿子节点，则不删除该节点，但是需要改变terminal_flag标志
	if(first->child_num > 0 ){
		first->terminal_flag = false;
		return true;
	}
	while(first->father && first->father->child_num == 1 && !first->father->terminal_flag ) {
        if(first->father == root)
            break;
        first = first->father;
    }
	dict_delete(first->father->dict,first->node_name);
	delete_tree_node(first);
	return true;
}
