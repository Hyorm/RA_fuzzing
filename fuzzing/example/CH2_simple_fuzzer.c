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

void file_fuzzer_create(char* program_name, char* file_name);
void file_fuzzer_invocking(char* program_name,char* buffer);

char* fuzzer(int max_length, int char_start, int char_range);

char* buffer;
char* fuzzer_output;

char fuzzer_read_input[BUFF_SIZE];

int max_ = 100, start_ = 32, range_ = 32;//100, 32, 32

int main(int argc, char** argv){

	
	//printf("fuzzer output: %s\n", fuzzer(max_, start_, range_));
	//printf("fuzzer output: %s\n", fuzzer(max_, (int)'a', 26));
	//free(buffer);
	
	for(int i = 0 ; i < 100; i++)	
		file_fuzzer_create(argv[1], "input.txt");
}


void file_fuzzer_create(char* program_name, char* file_name){

	FILE* fp;
	char buff[BUFF_SIZE];

	if(0<(fp = fopen(file_name, "w"))){
		fuzzer_output = fuzzer(max_, (int)'a', 26);
		//printf("fuzzer_output %s\n", fuzzer_output);
		free(buffer);
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

	//assert(strcmp(fuzzer_output, fuzzer_read_input)==0);

	file_fuzzer_invocking(program_name,fuzzer_read_input);

	free(buffer);
	free(fuzzer_output);
}

void file_fuzzer_invocking(char* program_name,char* buffer){

	char* cmd = malloc(sizeof(char)*(strlen(program_name)+strlen("gcc  ")+1));
	sprintf(cmd, "gcc %s", program_name);
	
	int returncode = 0;
	
	if(system(cmd) < 0){
                printf("system error\n");
                exit(1);
        }

        if(strcmp(cmd, "") !=0){
                memset(cmd, 0x00, sizeof(cmd));
        }
	
	cmd = malloc(sizeof(char)*(strlen(buffer) + strlen("./a.out  > _stdout")+1));
	sprintf(cmd, "./a.out %s > _stdout", buffer);


	if(returncode = system(cmd) < 0){
                printf("system error\n");
                exit(1);
	}
        if(strcmp(cmd, "") !=0){
                memset(cmd, 0x00, sizeof(cmd));
        }

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
