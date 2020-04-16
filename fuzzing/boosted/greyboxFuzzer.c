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

#include"greyboxFuzzer.h"

char* fuzz_GreyboxFuzzer(char* str){
	if(strlen(str)==0){
		printf("kill\n");
		exit(1);
	}
	return mutate(str);

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
                case 2 :                                                                                               rst = insert_random_character(seed);
                        //printf("insert %s\n", rst);
                        break;
                case 3 :
                        rst = flip_random_character(seed);
                        //printf("flip %s\n", rst);
                        break;
                default:
                        break;
        }

/*	int inv_num = 15;
        char inv_char[15] = {'<',';', '\t','\n','&','`','|','(', '\"', '\'', ')', '{', '}', ' ', '>'};
        int invalid = 0;

        char* inp_ = malloc(sizeof(char)*(strlen(rst)*2));
        for(int i = 0 ; i < strlen(rst)-1 ; i++){
                for(int j = 0; j < inv_num; j++){
                        if(rst[i] == inv_char[j]){
                                inp_[invalid++] = '\\';
                                break;
                        }
                }
                inp_[invalid++] = rst[i];
        }
        inp_[invalid] = '\0';
        return inp_;
	*/
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
