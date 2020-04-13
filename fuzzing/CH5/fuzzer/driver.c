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

#include"runner.h"
#include"fuzzer.h" 
#include"coverage.h"
#include"powerSolution.h"

int main(int argc, char** argv){

	Runner runner_base = {"PASS", "FAIL", "UNRESOLVED", run_Runner};

        //PrintRunner runner_print = {runner_base, run_PrintRunner};

        ProgramRunner runner_process = {runner_base, "./url", run_process_Program, run_Program};
        Fuzzer fzr = {"",fuzz_Fuzzer, run_Fuzzer, runs_Fuzzer};

        //RandomFuzzer rfzr = {fzr, 10, 100, 32, 32, run_RandomFuzzer, run_print_RandomFuzzer, run_program_RandomFuzzer, runs_RandomFuzzer, runs_print_RandomFuzzer, runs_program_RandomFuzzer,__init__RandomFuzzer, fuzz_RandomFuzzer};

        char** seed_input= (char**)malloc(sizeof(char*)*1);
        seed_input[0]= "http://www.google.com/search?q=fuzzing";
        Population pop;
        MutationFuzzer mf = {fzr, seed_input, 2, 10, 0, pop,__init__MutationFuzzer, reset_MutationFuzzer, mutate_MutationFuzzer, create_candidate_MutationFuzzer, fuzz_MutationFuzzer};
        mf = mf.__init__(mf);

        for(int i = 0; i< 10; i++){
                mf = mf.fuzz(mf);
        }
        printf("mf.populaion.pop_size: %d\n", mf.population.pop_size);

	PowerSchedule ps = {mf, assignEnergy_PowerSchedule, normalizedEnergy_PowerSchedule, choose_PowerSchedule};
	char* str =  ps.choose(ps, mf.population).seed;	
	printf("ps choose %s\n", str);
	printf("%s\n", runner_process.run(runner_process,str).value);
	Coverage cov;
	cov = getCoverage(cov, argv[1], str);
	printf("coverage = %.2f%\n", cov.coverage*100);

	
} 
