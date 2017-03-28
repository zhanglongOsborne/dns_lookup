#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dns_lookup.h"
#include"time.h"

char s[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

int random_int(int num){
	srand(rand());
	int ret = rand()%num;
    return ret;
}

char* str_rand(){
	int size = random_int(4)+1;	
	char *result = (char*)malloc(size+1);
	for(int i=0;i<size;i++){
		result[i] = s[random_int(26)];	
	}
    result[size] = '\0';
    //printf("%s\n",result);
	return result;
}

char* dns_rand(){
	int size = random_int(4)+2;
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
	//printf("the add dns is :%s\n",dns);
	return dns;
}

int main(){
	tree_node root;
	init_tree_node(&root,"root",0);
	root.dict=dict_init();
	if(strict_find_dns(&root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh") != NULL)
		printf("find the dns!!!!!!!!!!");
    
	for(int i=0;i<100000;i++){
		char *dns = dns_rand();
		add_dns(&root,dns);	
		free(dns);
	}
    delete_dns(&root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh");
    if(strict_find_dns(&root,"xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh") != NULL)
		printf("find the dns!!!!!!!!!!");
	return 1;
}
