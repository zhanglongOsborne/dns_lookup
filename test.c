#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dns_lookup.h"
#include"time.h"
#include<mcheck.h>

char s[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
unsigned int count = 0;
int random_int(int num){
	srand(rand()+count);
	int ret = rand()%num;
    count++;
    return ret;
}

char* str_rand(){
	int size = random_int(20)+1;	
	char *result = (char*)malloc(size+1);
	for(int i=0;i<size;i++){
		result[i] = s[random_int(26)];	
	}
    result[size] = '\0';
    //printf("%s\n",result);
	return result;
}

char* dns_rand(){
	int size = random_int(8)+2;
	char *s[size];
	int longth = 0;
	for(int i=0;i<size;i++){
		s[i] = str_rand();	
		longth = longth + strlen(s[i]);
	}
	char* dns = (char *)calloc(longth+size,sizeof(char));
	for(int i=0;i<size-1;i++){
		strcat(dns,s[i]);
		strcat(dns,".");
	}
	strcat(dns,s[size-1]);
	for(int i=0;i<size;i++){
		free(s[i]);
	}	
	printf("%s\n",dns);
	return dns;
}

int main(int argc , char **argv){
    //mtrace();
    int rule_num = atoi(argv[1]);
    int test_num = atoi(argv[2]);
	tree_node *root = (tree_node *)malloc(sizeof(tree_node));
	init_tree_node(root,"root",0);
	root->dict=dict_init();
	//add_dns(root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh");
    /**
    if(strict_find_dns(root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh") != NULL)
		printf("find the dns!!!!!!!!!!");
        */
    char *dns = NULL;
	for(int i=0;i<rule_num;i++){
		dns = dns_rand();
		add_dns(root,dns);	
        //delete_dns(root,dns);
		free(dns);
        dns = NULL;
	}
	for(int i=0;i<test_num;i++){
		dns = dns_rand();
		strict_find_dns(root,dns);	
		free(dns);
        dns = NULL;
	}
    //dict_status(root->dict);
    //dict_status(root->last_child->dict);
    //dict_status(root->last_child->last_child->dict);
    destory_tree_node(root);
    /*
    if(suffix_find_dns(root,"a.xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh") != NULL)
		printf("find the suffix dns!!!!!!!!!!");
        */
    //delete_dns(root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh");
    //muntrace();
    //free(root);
	root = NULL;
   	return 1;
}
