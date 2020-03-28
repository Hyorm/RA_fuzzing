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

typedef struct _Fuzzer{
	
	Runner fzrRunner;
	
	char* inp;

	char* (*fuzz)(struct _Fuzzer);

	ReturnRunner (*run)(struct _Fuzzer, Runner);

	ReturnRunner* (*runs)(struct _Fuzzer, /*PrintRunner*/Runner, int);


}Fuzzer;

typedef struct _RandomFuzzer{

	Fuzzer fzr;

	int min_length;
	int max_length;
	int char_start;
	int char_range;

	ReturnRunner (*run)(struct _RandomFuzzer, Runner);
	ReturnRunner* (*runs)(struct _RandomFuzzer, /*PrintRunner*/Runner, int);
	struct _RandomFuzzer (*__init__)(struct _RandomFuzzer, int, int, int, int);
	char* (*fuzz)(struct _RandomFuzzer);

}RandomFuzzer;

ReturnRunner run_Runner(Runner rnr, char* inp);

ReturnRunner run_PrintRunner(PrintRunner pr, char* inp);

ReturnRunProcess run_process_Program(ProgramRunner prnr, char* inp);
ReturnRunProcess run_Program(ProgramRunner prnr, char* inp);

char* fuzz_Fuzzer(Fuzzer fzr);
ReturnRunner run_Fuzzer(Fuzzer fzr, Runner rnr);
ReturnRunner* runs_Fuzzer(Fuzzer fzr, /*Print*/Runner pr, int trials);

ReturnRunner run_RandomFuzzer(RandomFuzzer rf, Runner rnr);
ReturnRunner* runs_RandomFuzzer(RandomFuzzer rf, /*Print*/Runner pr, int trials); 
RandomFuzzer __init__RandomFuzzer(RandomFuzzer rf, int min_length, int max_length, int char_start, int char_range);
char* fuzz_RandomFuzzer(RandomFuzzer rf);

char* buffer;

int main(int argv, char** argc){

	Runner runner_base = {"PASS", "FAIL", "UNRESOLVED", run_Runner};

	ReturnRunner rur = runner_base.run(runner_base, "Some Input");
	printf("%s === %s\n", rur.inp, rur.value);

	PrintRunner runner_print = {runner_base, run_PrintRunner};
	ReturnRunner rur_p = runner_print.run(runner_print, "Some Input");

	ProgramRunner runner_process = {runner_base, "echo", run_process_Program, run_Program};
	ReturnRunProcess rur_rp = runner_process.run(runner_process, "hello world");

	printf("%s %s\n", rur_rp.value, rur_rp.program_output);

	Fuzzer fzr = {runner_base, "",fuzz_Fuzzer, run_Fuzzer, runs_Fuzzer};

	RandomFuzzer rfzr = {fzr, 10, 100, 32, 32, run_RandomFuzzer, runs_RandomFuzzer,__init__RandomFuzzer, fuzz_RandomFuzzer};
	for(int i = 0; i < 10; i++){
		rfzr = rfzr.__init__(rfzr, 20, 20, 32, 32);
		printf("out %s\n", rfzr.fuzz(rfzr));
	}
	printf("out run %s\n", rfzr.run(rfzr, rfzr.fzr.fzrRunner).inp);
	rfzr.runs(rfzr, rfzr.fzr.fzrRunner, 10);
}

ReturnRunner run_RandomFuzzer(RandomFuzzer rf, Runner rnr){
	
	return rnr.run(rnr, rf.fuzz(rf));

}

ReturnRunner* runs_RandomFuzzer(RandomFuzzer rf, Runner rnr, int trials){

	ReturnRunner outcomes[trials];

	for(int i = 0;i < trials;i++){
		outcomes[i] = rf.run(rf, rnr);
		//printf("outcomes: %s\n", outcomes[i].value);
	}

	return outcomes;
}

RandomFuzzer __init__RandomFuzzer(RandomFuzzer rf, int min_length, int max_length, int char_start, int char_range){
	
	rf.min_length = min_length;
	rf.max_length = max_length;
	rf.char_start = char_start;
	rf.char_range = char_range;

	return rf;//is this necessary?

}

char* fuzz_RandomFuzzer(RandomFuzzer rf){

	struct timeval now;
	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	int string_length = rand()%(rf.max_length+1) + rf.min_length ;

	char* out = malloc(sizeof(char)*string_length);

	for(int i = 0; i<string_length; i++){
		char new_out= rand()%rf.char_range+rf.char_start;
		out[i] = new_out;
	}
	rf.fzr.inp = out;

	return out;

}

char* fuzz_Fuzzer(Fuzzer fzr){
	
	if(strcmp(fzr.inp, "")==0)return "";
	//Return fuzz input
	return fzr.inp;

}

ReturnRunner run_Fuzzer(Fuzzer fzr, Runner rnr){

	return rnr.run(rnr, fzr.fuzz(fzr));

}

ReturnRunner* runs_Fuzzer(Fuzzer fzr, /*Print*/Runner pr, int trials){

	ReturnRunner outcomes[trials];

	for(int i = 0;i < trials;i++){
		outcomes[i] = fzr.run(fzr, pr);
		//printf("outcomes: %s\n", outcomes[i].value);
	}

	return outcomes;
}

ReturnRunProcess run_process_Program(ProgramRunner prnr, char* inp){
	
	ReturnRunProcess rrp;
	FILE* fp;
	char* cmd_line = malloc(sizeof(char)*(strlen(inp)+strlen(prnr.program)+21));

	sprintf(cmd_line, "%s %s > _program_stdout_", prnr.program, inp);

	rrp.returncode = system(cmd_line);
	free(cmd_line);

	cmd_line = malloc(sizeof(char)*(strlen(inp)+strlen(prnr.program)+21));
	sprintf(cmd_line, "%s %s 2> _program_stderr_", prnr.program, inp);

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
		printf("file open error\n");
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
		printf("file open error\n");
		exit(1);
	}

	rrp.program_output = program_read_;
	rrp.program_error = program_read_error; 

	return rrp;
}

ReturnRunProcess run_Program(ProgramRunner prnr, char* inp){
	ReturnRunProcess rrp = prnr.run_process(prnr, inp);
	ReturnRunProcess rr;
	if(rrp.returncode == 0)
		rr.value = prnr.prgRnr.pass;
	else if(rrp.returncode < 0)
		rr.value = prnr.prgRnr.fail;
	else 
		rr.value = prnr.prgRnr.unresolved;

	rr.program_output = rrp.program_output;
	rr.program_error = rrp.program_error;

	return rr;
}

ReturnRunner run_Runner(Runner rnr, char* inp){
	ReturnRunner rr;
	rr.inp = inp;
	printf("inp %s\n", rr.inp);
	rr.value = rnr.unresolved;

	return rr;

}

ReturnRunner run_PrintRunner(PrintRunner pr, char* inp){
	ReturnRunner rr;
	rr.inp = inp;
	printf("%s\n", rr.inp);
	rr.value = pr.prRnr.unresolved;

	return rr;
}
