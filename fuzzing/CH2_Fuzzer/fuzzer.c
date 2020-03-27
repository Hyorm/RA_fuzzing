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

typedef struct _ReturnRunProcess{

	int returncode;
	char* value;
	char* program_output;
	char* program_error;

}ReturnRunProcess;

typedef struct _Runner{
	char* pass;// = "PASS";
	char* fail;// = "FAIL";
	char* unresolved;// = "UNRESOLVED";

	ReturnRunner (*run)(struct _Runner, char*);

}Runner;

typedef struct _PrintRunner{

	Runner prRnr;

	ReturnRunner (*run)(struct _PrintRunner, char*);

}PrintRunner;

typedef struct _ProgramRunner{
	
	Runner prgRnr;
	
	char* program;

	//Subprocess run
	ReturnRunProcess (*run_process)(struct _ProgramRunner, char*);

	ReturnRunProcess (*run)(struct _ProgramRunner, char*);

}ProgramRunner;

ReturnRunner run_Runner(Runner rnr, char* inp);
ReturnRunner run_PrintRunner(PrintRunner rnr, char* inp);

ReturnRunProcess run_process_Program(ProgramRunner rnr, char* inp);
ReturnRunProcess run_Program(ProgramRunner rnr, char* inp);

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
	/*
	ReturnRunner rur = runner_base.run(runner_base, fuzzer(max_, (int)'a', 26));
	printf("%s === %s\n", rur.inp, rur.value);

	PrintRunner runner_print = {runner_base, run_PrintRunner};
	ReturnRunner rur_p = runner_print.run(runner_print, "Some Input");
	*/
	ProgramRunner runner_process = {runner_base, "echo", run_process_Program, run_Program};
	ReturnRunProcess rur_rp = runner_process.run(runner_process, "hello world");

	printf("%s %s\n", rur_rp.value, rur_rp.program_output);
}

ReturnRunProcess run_process_Program(ProgramRunner rnr, char* inp){
	
	ReturnRunProcess rrp;
	FILE* fp;
	char* cmd_line = malloc(sizeof(char)*(strlen(inp)+strlen(rnr.program)+21));

	sprintf(cmd_line, "%s %s > _program_stdout_", rnr.program, inp);

	rrp.returncode = system(cmd_line);
	free(cmd_line);

	cmd_line = malloc(sizeof(char)*(strlen(inp)+strlen(rnr.program)+21));
	sprintf(cmd_line, "%s %s 2> _program_stderr_", rnr.program, inp);

	rrp.returncode = system(cmd_line);
	free(cmd_line);

	char program_read_[BUFF_SIZE*10];
	if(0<(fp = fopen("_program_stdout_", "r"))){
		while(fgets(program_read_, sizeof(program_read_), fp)){
			//printf("program stdout %s\n", program_read_);
		}
		fclose(fp);
	}
	else{
		printf("file open error");
		exit(1);
	}
	char program_read_error[BUFF_SIZE*10];

	if(0<(fp = fopen("_program_stderr_", "r"))){
		while(fgets(program_read_error, sizeof(program_read_error), fp)){
			//printf("program stderr %s\n", program_read_error);
		}
		fclose(fp);
	}
	else{
		printf("file open error");
		exit(1);
	}

	rrp.program_output = program_read_;
	rrp.program_error = program_read_error; 

	return rrp;
}

ReturnRunProcess run_Program(ProgramRunner rnr, char* inp){
	ReturnRunProcess rrp = rnr.run_process(rnr, inp);
	ReturnRunProcess rr;
	if(rrp.returncode == 0)
		rr.value = rnr.prgRnr.pass;
	else if(rrp.returncode < 0)
		rr.value = rnr.prgRnr.fail;
	else 
		rr.value = rnr.prgRnr.unresolved;

	rr.program_output = rrp.program_output;
	rr.program_error = rrp.program_error;

	return rr;
}

ReturnRunner run_Runner(Runner rnr, char* inp){
	ReturnRunner rr;
	rr.inp = inp;
	rr.value = rnr.unresolved;

	return rr;

}

ReturnRunner run_PrintRunner(PrintRunner rnr, char* inp){
	ReturnRunner rr;
	rr.inp = inp;
	printf("%s\n", rr.inp);
	rr.value = rnr.prRnr.unresolved;

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
