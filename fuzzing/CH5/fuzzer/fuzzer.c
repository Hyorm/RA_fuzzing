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
                outcomes[i] = fzr.run(fzr, pr);                                                                //printf("outcomes: %s\n", outcomes[i].value);
        }

        return outcomes;	

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

MutationFuzzer __init__MutationFuzzer(MutationFuzzer mf){

	return mf.reset(mf);

}

MutationFuzzer reset_MutationFuzzer(MutationFuzzer mf){

	Population popmf;
	popmf.se = malloc(sizeof(char*)*mf.pop_size);

        for(int i = 0; i< mf.pop_size;i++){

		popmf.se[i].seed =  malloc(sizeof(char)*(strlen(mf.seed[i])+1));
                popmf.se[i].seed = mf.seed[i];
        }

	popmf.pop_size = mf.pop_size;
	mf.seed_index = 0;

        mf.population = popmf;


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

        if(mf.population.pop_size != 1)pop = rand()%(mf.population.pop_size-1);

        char* candidate = mf.population.se[pop].seed;

        int trials = rand()%mf.max_mutations + mf.min_mutations;

        int pt = mf.population.pop_size+trials;

        //printf(">>>>>%d %d\n", mf.population.pop_size, pt);

        char buffer_pop[pt][BUFF_SIZE];

        int buf_idx = 0;


        for(int i = 0; i < mf.population.pop_size ; i++){
                memcpy(buffer_pop[i], mf.population.se[i].seed, strlen(mf.population.se[i].seed)+1);
                buf_idx++;	

        }

	for(int i = 0; i < trials ; i++){

                char* tmp_str = mf.mutate(candidate);
                memcpy(buffer_pop[buf_idx+i] , tmp_str, strlen(tmp_str)+1);
        }
	//printf("sese before\n");
	SeedEnergy* sese = malloc(sizeof(SeedEnergy)*pt);
	//printf("sese done\n");	
	for(int i = 0; i<pt; i++){
		sese[i].seed = malloc(sizeof(char)*(1+strlen(buffer_pop[i])));
		memcpy(sese[i].seed, buffer_pop[i], strlen(buffer_pop[i]));
	}	
	//printf("sese after\n");

        mf.population.se = sese;
	mf.population.pop_size = pt;

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

MutationFuzzer reset(MutationCoverageFuzzer mcf, MutationFuzzer mf, CoverLine covline){

	mcf.mf = mf;
	mf.reset(mf);
	
	mcf.coverage_seen = covline;

	mcf.population = mf.population;
	
	return mcf;
	
}

MutationCoverageFuzzer run(MutationCoverageFuzzer mcf, Runner rnr, CoverLine covline){

	ReturnRunner rr = mcf.run(mcf, rnr);
	CoverLine new_coverage = covline;

	if(strcmp(rr.value, "PASS")==0){
		for(int i = 0; i< new_coverage.idx; i++){
			for(int j = 0; j < coverage_seen.idx; j++)
				if(new_coverage.line[i] == mcf.coverage_seen[j])
		}
	}

}

MutationCoverageFuzzer run_print(MutationCoverageFuzzer mcf, PrintRunner prnr){

}

MutationCoverageFuzzer run_program(MutationCoverageFuzzer mcf, ProgramRunner pr){

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
                case 2 :                                                                                               rst = insert_random_character(seed);
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
