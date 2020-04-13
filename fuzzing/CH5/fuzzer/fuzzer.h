#ifndef __FUZZER_H__
#define __FUZZER_H__

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

typedef struct _SeedEnergy{

        char* seed;
        float energy;

}SeedEnergy;

typedef struct _Population{

        SeedEnergy* se;
        int pop_size;

}Population;

typedef struct _MutationFuzzer{

        Fuzzer fzr;

        char** seed;
        int min_mutations;//2
        int max_mutations;//10

        int seed_index;//0
        Population population;//seed

        struct _MutationFuzzer (*__init__)(struct _MutationFuzzer);
        struct _MutationFuzzer (*reset)(struct _MutationFuzzer);
        char* (*mutate)(char*);
        struct _MutationFuzzer (*create_candidate)(struct _MutationFuzzer);
        struct _MutationFuzzer (*fuzz)(struct _MutationFuzzer);

}MutationFuzzer;

typedef struct _MutationCoverageFuzzer{

	MutationFuzzer mf;
	CoverLine coverage_seen;
	Population population;

	struct _MutationCoverageFuzzer (*reset)(struct _MutationCoverageFuzzer, MutationFuzzer, CoverLine);
	struct _MutationCoverageFuzzer (*run)(struct _MutationCoverageFuzzer, Runner, CoverLine);
        struct _MutationCoverageFuzzer (*run_print)(struct _MutationCoverageFuzzer, PrintRunner, CoverLine);
        struct _MutationCoverageFuzzer (*run_program)(struct _MutationCoverageFuzzer, ProgramRunner, CoverLine);
	

}MutationCoverageFuzzer;

char* fuzz_Fuzzer(Fuzzer fzr);
ReturnRunner run_Fuzzer(Fuzzer fzr, Runner rnr);
ReturnRunner* runs_Fuzzer(Fuzzer fzr, /*Print*/Runner pr, int trials);

ReturnRunner run_RandomFuzzer(RandomFuzzer rf, Runner rnr);
ReturnRunner run_print_RandomFuzzer(RandomFuzzer rf, PrintRunner rnr);
ReturnRunProcess run_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner rnr);

ReturnRunner* runs_RandomFuzzer(RandomFuzzer rf, Runner rnr, int trials);
ReturnRunner* runs_print_RandomFuzzer(RandomFuzzer rf, PrintRunner pr, int trials);
ReturnRunProcess* runs_program_RandomFuzzer(RandomFuzzer rf, ProgramRunner pr, int trials);

RandomFuzzer __init__RandomFuzzer(RandomFuzzer rf, int min_length, int max_length, int char_start, int char_range);
char* fuzz_RandomFuzzer(RandomFuzzer rf);

MutationFuzzer __init__MutationFuzzer(MutationFuzzer mf);
MutationFuzzer reset_MutationFuzzer(MutationFuzzer mf);
char* mutate_MutationFuzzer(char* inp);
MutationFuzzer create_candidate_MutationFuzzer(MutationFuzzer mf);
MutationFuzzer fuzz_MutationFuzzer(MutationFuzzer mf);

MutationFuzzer reset(MutationCoverageFuzzer mcf, MutationFuzzer mf, CoverLine covline);
MutationCoverageFuzzer run(MutationCoverageFuzzer mcf, Runner rnr, CoverLine covline);
MutationCoverageFuzzer run_print(MutationCoverageFuzzer mcf, PrintRunner prnr, CoverLine covline);
MutationCoverageFuzzer run_program(MutationCoverageFuzzer mcf, ProgramRunner pr, CoverLine covline);

char* mutate(char* seed);
char* delete_random_character(char* seed);
char* insert_random_character(char* seed);
char* flip_random_character(char* seed);

#endif
