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

typedef struct _ReturnRunner{

	char* inp;
	char* value;

}ReturnRunner;

typedef struct _Runner{
	char* pass;// = "PASS";
	char* fail;// = "FAIL";
	char* unresolved;// = "UNRESOLVED";

	ReturnRunner (*run)(struct _Runner, char*);

}Runner;

typedef struct _PrintRunner{

	ReturnRunner (*run)(Runner, char*);

}PrintRunner;
/*
typedef struct _ProgramRunner{
	
	//Subprocess run


}ProgramRunner;
*/
ReturnRunner run_Runner(Runner rnr, char* inp);
ReturnRunner run_PrintRunner(Runner rnr, char* inp);

ReturnRunner rr;

void file_fuzzer(char* file_name);

char* fuzzer(int max_length, int char_start, int char_range);

char* buffer;
char* fuzzer_output;

char fuzzer_read_input[BUFF_SIZE];

int max_ = 1000, start_ = (int)'a', range_ = 26;//100, 32, 32

int main(int argv, char** argc){

	/*
	//printf("fuzzer output: %s\n", fuzzer(max_, start_, range_));
	printf("fuzzer output: %s\n", fuzzer(max_, (int)'a', 26));
	free(buffer);
	*/
	
	//file_fuzzer("input.txt");

	Runner runner_base = {"PASS", "FAIL", "UNRESOLVED", run_Runner};

	ReturnRunner rur = runner_base.run(runner_base, fuzzer(max_, (int)'a', 26));
	printf("%s === %s\n", rur.inp, rur.value);

	Runner runner_print = {"PASS", "FAIL", "UNRESOLVED", run_PrintRunner};
	ReturnRunner rur_p = runner_print.run(runner_print, "Some Input");

}

ReturnRunner run_Runner(Runner rnr, char* inp){
	
	rr.inp = inp;
	rr.value = rnr.unresolved;

	return rr;

}

ReturnRunner run_PrintRunner(Runner rnr, char* inp){

	rr.inp = inp;
	printf("%s\n", rr.inp);
	rr.value = rnr.unresolved;

	return rr;
}

void file_fuzzer(char* file_name){

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

	assert(strcmp(fuzzer_output, fuzzer_read_input)==0);

	free(buffer);
	free(fuzzer_output);
}

char* fuzzer(int max_length, int char_start, int char_range){

	srand((unsigned int)time(NULL));

	int string_length = rand()%(max_length+1);

	char* out = malloc(sizeof(char)*string_length);

	for(int i = 0; i<string_length; i++){
		char new_out= rand()%char_range+char_start;
		out[i] = new_out;
	}
	return out;
}
