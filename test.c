#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dns_lookup.h"

int main(){
	tree_node root;
	init_tree_node(&root,"root",0);
	root.dict=dict_init();
	add_dns(&root,"www.baidu.com");
	add_dns(&root,"www.baidu.com");
	add_dns(&root,"www.ucas.edu.cn");
	add_dns(&root,"a.www.baidu.com");
	if(strict_find_dns(&root,"www.baidu.com") != NULL)
		printf("find the dns strictly: %s","www.baidu.com");
	if(suffix_find_dns(&root,"b.a.www.baidu.com") != NULL)
		printf("find the dns suffix :%s","b.a.www.baidu.com");

	return 1;
}
