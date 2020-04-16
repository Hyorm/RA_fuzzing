#ifndef __BLACKBOXFUZZER_H__
#define __BLACKBOXFUZZER_H__

#define BUFF_SIZE 256

typedef struct _BlckboxFuzzer{

        char* (*fuzz)(char*);
	
}BlackboxFuzzer;

char* fuzz_BlackboxFuzzer( char* str);

char* mutate( char* seed);
char* delete_random_character( char* seed);
char* insert_random_character( char* seed);
char* flip_random_character( char* seed);

#endif
