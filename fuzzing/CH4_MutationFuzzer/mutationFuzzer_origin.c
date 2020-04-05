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
	int (*isNetloc)(struct _URL, char*);
	int (*isPath)(struct _URL, char*);
	int (*isParams)(struct _URL, char*);
	int (*isQuery)(struct _URL, char*);
	int (*isFragment)(struct _URL, char*);

}URL;

int urlParse_URL(URL url, char* snp);
int isScheme_URL(URL url, char* snp);
int isNetloc_URL(URL url, char* snp);
int isPath_URL(URL url, char* snp);
int isParams_URL(URL url, char* snp);
int isQuery_URL(URL url, char* snp);
int isFragment_URL(URL url, char* snp);

char* mutate(char* seed); 
char* delete_random_character(char* seed);
char* insert_random_character(char* seed);
char* flip_random_character(char* seed);

void file_fuzzer(char* file_name);

char* fuzzer(int max_length, int char_start, int char_range);

char* fuzzer_output;

char fuzzer_read_input[BUFF_SIZE];

int max_ = 100, start_ = 32, range_ = 32;//100, 32, 32

int main(int argv, char** argc){

	
	//printf("fuzzer output: %s\n", fuzzer(max_, start_, range_));
	//printf("fuzzer output: %s\n", fuzzer(1000, (int)'a', 26));
	
	//printf("fuzzer output: %s\n", fuzzer(max_, 32, 96));

	URL url = {" ", " ", " ", " ", " ", " ", urlParse_URL, isScheme_URL, isNetloc_URL, isPath_URL, isParams_URL, isQuery_URL, isFragment_URL};

	int count = 0;

	for(int i = 0; i < 1000 ; i++){
		if(url.urlParse(url, mutate("http://www.google.com/search?q=fuzzing")))
			count++;//printf("Success\n");

	}
	printf("success count: %d / 1000\n", count);
	
	
	//file_fuzzer("input.txt");
}

char* mutate(char* seed){

	struct timeval now;
	
	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	int mutator = rand()%3+1;

	char* rst;

	switch (mutator){
	
		case 1 : 
			rst = delete_random_character(seed);
			//printf("delete %s\n", rst);
			break;
		case 2 : 
			rst = insert_random_character(seed);
			//printf("insert %s\n", rst);
			break;
		case 3 : 
			rst = flip_random_character(seed);
			//printf("flip %s\n", rst);
			break;
		default:
			break;
	}

	return rst;
}

char* delete_random_character(char* seed){

	struct timeval now;

	int pos = 0;

	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	if(strcmp(seed, "")== 0) return seed;

	pos = rand()%(strlen(seed)-1);

	char* out = malloc(sizeof(char)*strlen(seed));

	for(int i = 0; i < pos ; i++)
		out[i] = seed[i];

	for(int i = pos ; i < strlen(seed)-1 ; i++)
		out[i] = seed[i+1];

	return out;
}

char* insert_random_character(char* seed){

	struct timeval now;

	int pos = 0;

	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	pos = rand()%(strlen(seed));
	
	char* out = malloc(sizeof(char)*strlen(seed)+1);

	for(int i = 0; i < pos ; i++)
		out[i] = seed[i];
	
	out[pos] = (char)rand()%127 + 32;
	
	for(int i = pos+1; i < strlen(seed)+1 ; i++)
		out[i] = seed[i-1];

	return out;
}

char* flip_random_character(char* seed){

	struct timeval now;

	int pos = 0, bit;
	
	char c;

	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	if(strcmp(seed, "")== 0) return seed;

	pos = rand()%(strlen(seed)-1);

	c = seed[pos];

	char* out = malloc(sizeof(char)*strlen(seed));

	for(int i = 0; i < pos ; i++)
		out[i] = seed[i];                                                              
	
	bit = 1 << rand()%6;
	out[pos] = (char)(bit ^ c); 

	for(int i = pos + 1 ; i < strlen(seed)+1 ; i++)
		out[i] = seed[i-1];

	return out;
	

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

int isNetloc_URL(URL url, char* snp){
	
	return 0;

}

int isPath_URL(URL url, char* snp){
	
	return 0;

}

int isParams_URL(URL url, char* snp){

	return 0;

}

int isQuery_URL(URL url, char* snp){

	return 0;

}

int isFragment_URL(URL url, char* snp){

	return 0;

}

void file_fuzzer(char* file_name){

	FILE* fp;
	char buff[BUFF_SIZE];

	if(0<(fp = fopen(file_name, "w"))){
		fuzzer_output = fuzzer(max_, (int)'a', 26);
		//printf("fuzzer_output %s\n", fuzzer_output);
		fprintf(fp, fuzzer_output,strlen(fuzzer_output));
		fclose(fp);
	}
	else{
		printf("file open error");
	}

	if(0<(fp = fopen(file_name, "r"))){	
		while(fgets(fuzzer_read_input, sizeof(fuzzer_read_input), fp)){
			//printf("fuzzer_file_input %s\n", fuzzer_read_input);
		}

		fclose(fp);
	}
	else{
		printf("file open error");
	}

	assert(strcmp(fuzzer_output, fuzzer_read_input)==0);

	free(fuzzer_output);
}

char* fuzzer(int max_length, int char_start, int char_range){

	struct timeval now;
	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	int string_length = rand()%(max_length+1);

	char* out = malloc(sizeof(char)*string_length);

	for(int i = 0; i<string_length; i++){
		char new_out= rand()%char_range+char_start;
		out[i] = new_out;
	}
	return out;
}
