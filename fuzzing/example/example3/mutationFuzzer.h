#ifndef __MUTATIONFUZZER_H__
#define __MUTATIONFUZZER_H__

#define BUFF_SIZE 256

typedef struct _MutationFuzzer{

        char* (*fuzz)(char*);
	
}MutationFuzzer;

char* fuzz_MutationFuzzer( char* str);

char* mutate( char* seed);
char* delete_random_character( char* seed);
char* insert_random_character( char* seed);
char* flip_random_character( char* seed);

#endif
