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
/*//test
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

bool http_program(char* url);
//test_end*/

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

typedef struct _FunctionRunner{

	Runner fRnr;

	char* function;

}FunctionRunner;

typedef struct _Fuzzer{
	/*
	Runner fzrRunner;
	PrintRunner prtRunner;
	ProgramRunner prgRnr;
	*/
	char* inp;

	char* (*fuzz)(struct _Fuzzer);

	ReturnRunner (*run)(struct _Fuzzer, Runner);

	ReturnRunner* (*runs)(struct _Fuzzer, /*PrintRunner*/Runner, int);


}Fuzzer;

typedef struct _RandomFuzzer{

	Fuzzer fzr;

	int min_length;//0
	int max_length;//100
	int char_start;//32 or (int)'a'
	int char_range;//32

	ReturnRunner (*run)(struct _RandomFuzzer, Runner);
	ReturnRunner (*run_print)(struct _RandomFuzzer, PrintRunner);
	ReturnRunProcess (*run_program)(struct _RandomFuzzer, ProgramRunner);
	
	ReturnRunner* (*runs)(struct _RandomFuzzer, Runner, int);
	ReturnRunner* (*runs_print)(struct _RandomFuzzer, PrintRunner, int);
	ReturnRunProcess* (*runs_program)(struct _RandomFuzzer, ProgramRunner, int);

	struct _RandomFuzzer (*__init__)(struct _RandomFuzzer, int, int, int, int);
	char* (*fuzz)(struct _RandomFuzzer);

}RandomFuzzer;

typedef struct _MutationFuzzer{
	
	Fuzzer fzr;

	char** seed;
	int min_mutations;//2
	int max_mutations;//10

	int seed_index;//0
	char** population;//seed
	int pop_size;

	struct _MutationFuzzer (*__init__)(struct _MutationFuzzer);
	struct _MutationFuzzer (*reset)(struct _MutationFuzzer);
	char* (*mutate)(char*);
	struct _MutationFuzzer (*create_candidate)(struct _MutationFuzzer);
	struct _MutationFuzzer (*fuzz)(struct _MutationFuzzer);

}MutationFuzzer;

MutationFuzzer __init__MutationFuzzer(MutationFuzzer mf);
MutationFuzzer reset_MutationFuzzer(MutationFuzzer mf);
char* mutate_MutationFuzzer(char* inp);
MutationFuzzer create_candidate_MutationFuzzer(MutationFuzzer mf);
MutationFuzzer fuzz_MutationFuzzer(MutationFuzzer mf);

ReturnRunner run_Runner(Runner rnr, char* inp);

ReturnRunner run_PrintRunner(PrintRunner pr, char* inp);

ReturnRunProcess run_process_Program(ProgramRunner prnr, char* inp);
ReturnRunProcess run_Program(ProgramRunner prnr, char* inp);

char* fuzz_Fuzzer(Fuzzer fzr);
ReturnRunner run_Fuzzer(Fuzzer fzr, Runner rnr);
ReturnRunner* runs_Fuzzer(Fuzzer fzr, /*Print*/Runner pr, int trials);

ReturnRunner run_RandomFuzzer(RandomFuzzer rf, Runner rnr);
ReturnRunner run_print_RandomFuzzer(RandomFuzzer rf, PrintRunner rnr); 
ReturnRunProcess run_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner rnr); 

ReturnRunner* runs_RandomFuzzer(RandomFuzzer rf, Runner pr, int trials);
ReturnRunner* runs_print_RandomFuzzer(RandomFuzzer rf, PrintRunner pr, int trials);
ReturnRunProcess* runs_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner pr, int trials);

RandomFuzzer __init__RandomFuzzer(RandomFuzzer rf, int min_length, int max_length, int char_start, int char_range);
char* fuzz_RandomFuzzer(RandomFuzzer rf);

char* mutate(char* seed);
char* delete_random_character(char* seed);
char* insert_random_character(char* seed);
char* flip_random_character(char* seed);

int main(int argv, char** argc){

	Runner runner_base = {"PASS", "FAIL", "UNRESOLVED", run_Runner};

	//ReturnRunner rur = runner_base.run(runner_base, "Some Input");
	//printf("%s === %s\n", rur.inp, rur.value);

	PrintRunner runner_print = {runner_base, run_PrintRunner};
	//ReturnRunner rur_p = runner_print.run(runner_print, "Some Input");

	ProgramRunner runner_process = {runner_base, "echo", run_process_Program, run_Program};
	//ReturnRunProcess rur_rp = runner_process.run(runner_process, "hello world");

	//printf("%s %s\n", rur_rp.value, rur_rp.program_output);

	Fuzzer fzr = {"",fuzz_Fuzzer, run_Fuzzer, runs_Fuzzer};

	RandomFuzzer rfzr = {fzr, 10, 100, 32, 32, run_RandomFuzzer, run_print_RandomFuzzer, run_program_RandomFuzzer, runs_RandomFuzzer, runs_print_RandomFuzzer, runs_program_RandomFuzzer,__init__RandomFuzzer, fuzz_RandomFuzzer};
	
	//rfzr = rfzr.__init__(rfzr, 20, 20, 32, 32);
	//for(int i = 0; i < 10; i++){
	//	printf("out %s\n", rfzr.fuzz(rfzr));
	//}
	/*
	printf("program  %s\n", rfzr.run_program(rfzr, runner_process).program_output);
	rfzr.runs_print(rfzr, runner_print, 10);
	*/
	char** seed_input= (char**)malloc(sizeof(char*)*1);
	seed_input[0]= "http://www.google.com/search?q=fuzzing";
	char** population_ = (char**)malloc(sizeof(char*)*1);
	population_[0] = "http://www.google.com/search?q=fuzzing";
	MutationFuzzer mf = {fzr, seed_input, 2, 10, 0, population_, 1,__init__MutationFuzzer, reset_MutationFuzzer, mutate_MutationFuzzer, create_candidate_MutationFuzzer, fuzz_MutationFuzzer};
	mf = mf.__init__(mf);
	for(int i = 0; i< 10; i++){
		mf = mf.fuzz(mf);
		printf("fuzz %s\n",mf.fzr.inp);
	}	
}

MutationFuzzer __init__MutationFuzzer(MutationFuzzer mf){
	
	return mf.reset(mf);

}

MutationFuzzer reset_MutationFuzzer(MutationFuzzer mf){

	if(strcmp(mf.population[0], "") != 0){
		free(mf.population);
	}	
	mf.population = mf.seed;
	mf.seed_index = 0;
	mf.pop_size = sizeof(mf.seed)/sizeof(char*);

	return mf;
}

char* mutate_MutationFuzzer(char* inp){

	return mutate(inp);	

}

MutationFuzzer create_candidate_MutationFuzzer(MutationFuzzer mf){

	struct timeval now;

	gettimeofday(&now, NULL);
	srand((unsigned int)now.tv_usec);

	int pop = 0;

	if(mf.pop_size != 1)pop = rand()%(mf.pop_size-1);

	printf("pop num %d\n", pop);

	char* candidate = mf.population[pop];

	int trials = rand()%mf.max_mutations + mf.min_mutations;

	char** buffer_pop;
	int buf_idx = 0;
	buffer_pop = (char**)malloc(sizeof(char*)*(mf.pop_size+trials));
	for(int i = 0; i < mf.pop_size ; i++){
		buffer_pop[i] = (char*)malloc(sizeof(char)*strlen( mf.population[i]));
		strcpy(buffer_pop[i], mf.population[i]);
		buf_idx++;
	
	}
	for(int i = 0; i < trials ; i++){
		
		candidate = mf.mutate(candidate);
		buffer_pop[buf_idx] = (char*)malloc(sizeof(char)*strlen(candidate));
		strcpy(buffer_pop[buf_idx], candidate);
		printf("candidate: %s\n", buffer_pop[buf_idx]);
		buf_idx++;
	}

	//add pop
	free(mf.population);
	
	mf.population = (char**)malloc(sizeof(char*)*buf_idx);
	mf.pop_size = 0;
	for(int i = 0; i < buf_idx ; i++){
		mf.population[i] = buffer_pop[i];
		mf.pop_size++;

	}
	
	mf.fzr.inp = candidate;

	return mf;
}

MutationFuzzer fuzz_MutationFuzzer(MutationFuzzer mf){

	if(mf.seed_index < (sizeof(mf.seed)/sizeof(char*))){
		mf.fzr.inp = mf.seed[mf.seed_index];
		mf.seed_index = mf.seed_index + 1;
	}
	else
		mf = mf.create_candidate(mf);
	
	return mf;

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
/*//test
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
//test end*/
ReturnRunner run_RandomFuzzer(RandomFuzzer rf, Runner rnr){
	
	return rnr.run(rnr, rf.fuzz(rf));

}

ReturnRunner run_print_RandomFuzzer(RandomFuzzer rf, PrintRunner rnr){

	return rnr.run(rnr, rf.fuzz(rf));

}

ReturnRunProcess run_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner rnr){

	return rnr.run(rnr, rf.fuzz(rf));

}

ReturnRunner* runs_RandomFuzzer(RandomFuzzer rf, Runner rnr, int trials){

	ReturnRunner outcomes[trials];

	for(int i = 0;i < trials;i++){
		outcomes[i] = rf.run(rf, rnr);
		printf("runs_RandomFuzzer: %s\n", outcomes[i].inp);
	}

	return outcomes;
}

ReturnRunner* runs_print_RandomFuzzer(RandomFuzzer rf, PrintRunner pr, int trials){

	ReturnRunner outcomes[trials]; 

	for(int i = 0;i < trials;i++){
		outcomes[i] = rf.run_print(rf, pr);
	}
	return outcomes;

}

ReturnRunProcess* runs_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner pr, int trials){

	ReturnRunProcess outcomes[trials];

	for(int i = 0;i < trials;i++){
		outcomes[i] = rf.run_program(rf, pr);
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
	printf("fuzz: %s\n", out);
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
		rrp.program_output = program_read_;
	}
	else{
		printf("file open error\n");
		rrp.program_output = "file open error";
	}
	char program_read_error[BUFF_SIZE*10];

	if(0<(fp = fopen("_program_stderr_", "r"))){
		while(fgets(program_read_error, sizeof(program_read_error), fp)){
			//printf("program stderr %s\n", program_read_error);
		}
		fclose(fp);
		rrp.program_error = program_read_error;
	}
	else{
		printf("file open error\n");
		rrp.program_error = "file open error";
	}

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
