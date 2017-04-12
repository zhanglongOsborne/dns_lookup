#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dns_lookup.h"
#include"time.h"

char s[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
typedef struct dns{
	char *dns;
	int length;
}dns_t;
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

dns_t* dns_rand(){
	int size = random_int(8)+2;
	char *s[size];
	int longth = 0;
	dns_t *dns_entry = (dns_t*)malloc(sizeof(dns_t));
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
	dns_entry->dns = dns;
	dns_entry->length = longth+size-1;
	for(int i=0;i<size;i++){
		free(s[i]);
	}	
	//printf("%s\n",dns);
	return dns_entry;
}

int main(int argc,char **argv){
	//int rule_num = atoi(argv[1]);
	//int test_num = atoi(argv[3]);
	//int add_flag = atoi(argv[2]);
	tree_node *root = (tree_node *)malloc(sizeof(tree_node));
	init_tree_node(root,"root",0,3,0);
	root->dict=dict_init();
	/**
	char *dns_test = "xuyds.q.ilq.y.imawhwle.vjgre.hn.dwct.wdh";
	add_dns(root,dns_test,strlen(dns_test),'.');
	if(strict_find_dns(root,dns_test,strlen(dns_test),'.') != NULL)
		printf("find the dns!!!!!!!!!!");
	**/
	FILE *fp = fopen("dns_log_200W","r");
	char **dns_line = (char **)malloc(4000000*sizeof(char *));
	for(int i=0;i<4000000;i++){
		dns_line[i] = (char *)malloc(200);
		memset(dns_line[i],0,200);
		fgets(dns_line[i],199,fp);
	}

	for(int i=0;i<1000000;i++){
		add_dns(root,dns_line[i],strlen(dns_line[i]),'.');	
	}
	/**
	for(int i=0;i<4000000;i++){
		strict_find_dns(root,dns_line[i],strlen(dns_line[i]),'.');
	}
	**/
	/**
	dns_t *dns = NULL;
	for(int i=0;i<rule_num;i++){
		dns = dns_rand();
		if(add_flag == 1)
			add_dns(root,dns->dns,dns->length,'.');	
		free(dns->dns);
		dns->dns = NULL;
		free(dns);
		dns = NULL;
	}
	count = 0;
	for(int i=0;i<test_num;i++){
		dns = dns_rand();
		if(strict_find_dns(root,dns->dns,dns->length,'.'))
			printf("find the dns:%s\n",dns->dns);
		free(dns->dns);
		dns->dns = NULL;
		free(dns);
		dns = NULL;
	}
	**/
	/**
	if(strict_suffix_find_dns(root,dns_test,strlen(dns_test),'.') != NULL)
		 printf("find the suffix dns!!!!!!!!!!");
	delete_dns(root,dns_test,strlen(dns_test),'.');
	if(strict_find_dns(root,dns_test,strlen(dns_test),'.') != NULL)
		printf("find the dns!!!!!!!!!!");
		**/
	destory_tree_node(root);
	root = NULL;
	return 1;
}
