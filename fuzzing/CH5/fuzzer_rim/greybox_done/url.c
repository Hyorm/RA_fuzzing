#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>
#include<time.h>
#include<setjmp.h>
#include<string.h>
#include<ctype.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFF_SIZE 1024

typedef struct _URL{

	char* scheme;
	char* netloc;
	char* path;
	char* params;
	char* query;
	char* fragment;

	int (*urlParse)(struct _URL, char*);
	int (*isScheme)(struct _URL, char*);

}URL;

int urlParse_URL(URL url, char* snp);
int isScheme_URL(URL url, char* snp);

int main(int argc, char** argv){

	URL url = {" ", " ", " ", " ", " ", " ", urlParse_URL, isScheme_URL};

	if(url.urlParse(url, argv[1])){
		printf(" [%s] is URL\n", argv[1]);
		return 0;
	}
	return -1;
}

int urlParse_URL(URL url, char* snp){

	int end;

	for(int i = 1; i < strlen(snp); i++){
		if((snp[i] == ':')&&(snp[i+1] == '/')&&(snp[i+2] == '/')){
			char* ptr = malloc(sizeof(char)*i);
			for(int j = 0; j < i; j++){
				ptr[j] = snp[j];
			}
			strncpy(ptr, snp, i);
			return url.isScheme(url, ptr);
			
		}
	}
	return 0;
}

int isScheme_URL(URL url, char* snp){
	
	int scheme_num = 4;
	char* scheme[] = {"http", "https", "ftp", "file"};

	for(int i = 0; i < scheme_num ; i++){
	
		if(strcmp(scheme[i], snp)==0){
			url.scheme = snp;
			//printf("url scheme = %s\n", url.scheme);
			return 1;
		}
	}

	return 0;
}

