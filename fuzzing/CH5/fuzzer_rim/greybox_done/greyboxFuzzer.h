#ifndef __GREYBOXFUZZER_H__
#define __GREYBOXFUZZER_H__

#define BUFF_SIZE 256

typedef struct _GreyboxFuzzer{

        char* (*fuzz)(char*);
	
}GreyboxFuzzer;

char* fuzz_GreyboxFuzzer( char* str);

char* mutate( char* seed);
char* delete_random_character( char* seed);
char* insert_random_character( char* seed);
char* flip_random_character( char* seed);

#endif
