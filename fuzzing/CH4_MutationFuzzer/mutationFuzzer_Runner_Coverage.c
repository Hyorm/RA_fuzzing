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

#define BUFF_SIZE 256

char** buffer;
int buffer_num; 
typedef struct _LINECODE{

	char** code;
	int line;

}LINECODE;

typedef struct _COVLINE{

	int* line;
	int idx;
	int non_cover_line;

}COVLINE;

typedef struct _COVERAGE{

        char* filename;
        COVLINE cov_lines;
        LINECODE cov_code;
        float coverage;
        char* input;

}COVERAGE;

typedef struct _COMPCOV{

	COVLINE onlya;
	COVLINE onlyb; 
	COVLINE intersecion;

}COMPCOV;

typedef struct _POPULATION{

	COVERAGE* coverage_pop;
	
}POPULATION;

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

LINECODE readfile(char* filename);
COVERAGE getCoverage(COVERAGE cov, char* filename, char* input); 
COMPCOV cmpCov(COVERAGE a, COVERAGE b);
void sys(char* cmd);
COVLINE getCovLine(COVERAGE cov);

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

int main(int argc, char** argv){
	buffer_num = 0;

	Runner runner_base = {"PASS", "FAIL", "UNRESOLVED", run_Runner};

	PrintRunner runner_print = {runner_base, run_PrintRunner};

	ProgramRunner runner_process = {runner_base, "echo", run_process_Program, run_Program};

	Fuzzer fzr = {"",fuzz_Fuzzer, run_Fuzzer, runs_Fuzzer};

	RandomFuzzer rfzr = {fzr, 10, 100, 32, 32, run_RandomFuzzer, run_print_RandomFuzzer, run_program_RandomFuzzer, runs_RandomFuzzer, runs_print_RandomFuzzer, runs_program_RandomFuzzer,__init__RandomFuzzer, fuzz_RandomFuzzer};
	
	char** seed_input= (char**)malloc(sizeof(char*)*1);
	seed_input[0]= "http://www.google.com/search?q=fuzzing";
	char** population_ = (char**)malloc(sizeof(char*)*1);
	population_[0] = "http://www.google.com/search?q=fuzzing";
	MutationFuzzer mf = {fzr, seed_input, 2, 10, 0, population_, 1,__init__MutationFuzzer, reset_MutationFuzzer, mutate_MutationFuzzer, create_candidate_MutationFuzzer, fuzz_MutationFuzzer};
	mf = mf.__init__(mf);

	
	for(int i = 0; i< 10; i++){
		mf = mf.fuzz(mf);
	}
	printf("mf.pop_size: %d\n", mf.pop_size);

	COVERAGE* a = malloc(sizeof(COVERAGE)*mf.pop_size);

	for(int j = 0 ; j < mf.pop_size ; j++){
		printf(" %d mf.pop %s\n", j, mf.population[j]);
		a[j] = getCoverage(a[j], argv[1], mf.population[j]);
	}
	cmpCov(a[mf.pop_size-1], a[0]);
}

COVLINE getCovLine(COVERAGE cov){

	COVLINE covline;
	int* covLine = malloc(sizeof(int)*(cov.cov_code.line));
	int lineIdx = 0;
	covline.non_cover_line = 0;
	char orgcov[BUFF_SIZE];
	for(int i = 0 ; i < cov.cov_code.line ; i++){
		strcpy(orgcov, cov.cov_code.code[i]);
		char* ptr; 
		ptr = strtok(orgcov, ":");
		if((ptr !=NULL) && (strcmp(ptr, "    #####") != 0)){
			if((strcmp(ptr, "        -") == 0))
				covline.non_cover_line++;
			else{
				ptr = strtok(NULL, ":");
				if(ptr !=NULL){
					covLine[lineIdx++] = atoi(ptr);
				}
			}	
		}
	}
	covline.line = covLine;
	covline.idx = lineIdx;
	return covline;

}

void sys(char* cmd){

	if(system(cmd) < 0){
		printf("system error\n");
		exit(1);
	}
	if(strcmp(cmd, "") !=0){
		memset(cmd, 0x00, sizeof(cmd));
	}	
}

COVERAGE getCoverage(COVERAGE cov, char* filename, char* input){
	COVLINE covline;
	cov.filename = filename;

	int inv_num = 15;
	char inv_char[15] = {'<',';', '\t','\n','&','`','|','(', '\"', '\'', ')', '{', '}', ' ', '>'};
	int invalid = 0;
	char* inp_ = malloc(sizeof(char)*(strlen(input)*2));
	for(int i = 0 ; i < strlen(input) ; i++){
		for(int j = 0; j < inv_num; j++){
			if(input[i] == inv_char[j]){
				inp_[invalid++] = '\\';	
				break;
			}
		}
		inp_[invalid++] = input[i];
	}
	inp_[invalid] = '\0';
	cov.input = inp_;

	char* cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen("gcc -fprofile-arcs -ftest-coverage ") + 1));

	sprintf(cmd, "gcc -fprofile-arcs -ftest-coverage %s", cov.filename);
	sys(cmd);
	
	cmd = malloc(sizeof(char)*(strlen(cov.input)+strlen("./a.out ")+1));
	sprintf(cmd, "./a.out %s", cov.input);
	sys(cmd);
		
	cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen("gcov ") + 1));
	sprintf(cmd, "gcov %s", cov.filename);
	
	sys(cmd);
	
	cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen(".gcov") + 1));
	sprintf(cmd, "%s.gcov", cov.filename); 
	
	cov.cov_code = readfile(cmd);

	cov.cov_lines = getCovLine(cov);
	
	if(cov.cov_lines.idx != 0)
		cov.coverage = (float)cov.cov_lines.idx/(float)(cov.cov_code.line - cov.cov_lines.non_cover_line);
	else 
		cov.coverage = 0.0;
	printf("coverage = %f\%\n", cov.coverage * 100);	
	return cov;
}

COMPCOV cmpCov(COVERAGE a, COVERAGE b){

	COMPCOV comp;
	
	comp.onlya.line = malloc(sizeof(int)* a.cov_lines.idx);
	comp.onlya.idx = 0;

	comp.onlyb.line = malloc(sizeof(int)* b.cov_lines.idx);
	comp.onlyb.idx = 0;

	if(a.cov_lines.idx > b.cov_lines.idx)
		comp.intersecion.line = malloc(sizeof(int)* b.cov_lines.idx);
	else 	
		comp.intersecion.line = malloc(sizeof(int)* a.cov_lines.idx);

	comp.intersecion.idx = 0;

	for(int i = 0; i < a.cov_lines.idx ; i++){
		int inflag = 0;
		for(int j = 0 ; j < b.cov_lines.idx ; j++){
			if(a.cov_lines.line[i] == b.cov_lines.line[j]){
				inflag = 1;
				break;
			}
		}
		if(inflag == 1)comp.intersecion.line[comp.intersecion.idx++] = a.cov_lines.line[i];
		else comp.onlya.line[comp.onlya.idx++] = a.cov_lines.line[i];
	}

	for(int i = 0; i < b.cov_lines.idx ; i++){
		int inflag = 0;
		for(int j = 0 ; j < a.cov_lines.idx ; j++){
			if(a.cov_lines.line[j] == b.cov_lines.line[i]){ 
				inflag = 1;
				break;
			}
		}
		if(inflag == 1){
			int othflag = 0;
			for(int j = 0; j < comp.intersecion.idx ; j++){
				if(comp.intersecion.line[j]==b.cov_lines.line[i])
					othflag++;
			}
			if(othflag == 0)comp.intersecion.line[comp.intersecion.idx++] = b.cov_lines.line[i];
		}
		else comp.onlyb.line[comp.onlyb.idx++] = b.cov_lines.line[i];
	}

	printf("size : %d comp intersecion: \n", comp.intersecion.idx);
	for(int i = 0; i < comp.intersecion.idx; i++)		
		printf("%d ", comp.intersecion.line[i]);
	printf("\n");
	
	printf("size: %d comp a.cov \n:", comp.onlya.idx);
	for(int i = 0; i < comp.onlya.idx ; i++)
		 printf("%d ", comp.onlya.line[i]);
	printf("\n"); 

	printf("size: %d comp b.cov : \n", comp.onlyb.idx);
	for(int i = 0; i < comp.onlyb.idx ; i++)
		printf("%d ", comp.onlyb.line[i]);
	
	printf("\n"); 
	
	return comp;
	
}

LINECODE readfile(char* filename){

	if(buffer_num != 0){
		for(int i = 0; i< buffer_num; i++)
			free(buffer[i]);
		free(buffer);
	}

	LINECODE linecode;

	FILE* fp;


	char buff[BUFF_SIZE];

	int linenum = 0;
	if(0<(fp = fopen(filename, "r"))){
		while(!feof(fp)){

			fgets(buff, BUFF_SIZE-1, fp);

			linenum++;
		}
		fclose(fp);
		
	}
	else{

		printf("file open error\n");
		exit(1);
	}
	
	buffer = malloc(linenum * sizeof(char*));
	for(int i = 0; i< linenum ; i++)
		buffer[i] = (char*)malloc(sizeof(char)*BUFF_SIZE);
	if(0<(fp = fopen(filename, "r"))){
		
		int i = 0;
		for(int i = 0; i< linenum ; i++){	
			fgets(buffer[i], BUFF_SIZE-1, fp);
			//printf("%d %s\n", i-1,buffer[i-1]);
		
		}
		fclose(fp);
	}
	else{
		printf("file open error\n");
		exit(1);
	}
	
	linecode.code = buffer;
	linecode.line = linenum-1;
	buffer_num = linenum;
	return linecode;

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
	rr.value = pr.prRnr.unresolved;

	return rr;
}
