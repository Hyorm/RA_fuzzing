#ifndef __GREYBOXRUNNER_H__
#define __GREYBOXRUNNER_H__

#define MAX_SIZE 1024

typedef struct _LineCode{

        char** code;
        int line;

}LineCode;

typedef struct _CoverLine{

        int* line;
        int idx;
        int non_cover_line;
	int value;
	int acc;

}CoverLine;

typedef struct _Coverage{

	float coverage;
	CoverLine cov_line;

}Coverage;

typedef struct _Input{
	
	char* input;
	int value;
	float energy;
	float acc;

}Input;

typedef struct _Population{

	Input* input;
	int size;
	Coverage cov;

}Population;

typedef struct _CmpCoverage{

        CoverLine onlya;
        CoverLine onlyb;
        CoverLine intersecion;

}CmpCoverage;

typedef struct _greyboxRunner{

	Population pop;
	
	struct _greyboxRunner (*run)(struct _greyboxRunner, char*, char**, int, int, int, int);	

}greyboxRunner;

struct _greyboxRunner run_greyboxRunner(greyboxRunner gr, char* filename, char** seed, int seed_size, int min_mtt, int max_mtt, int flag);

float AFLFastSchedule(Input* input, int size);

LineCode readfile(char* filename);
Coverage getCoverage(Coverage cov, char* filename, char* input);
CmpCoverage getCmpCoverage(Coverage a, Coverage b);
CoverLine getCoverLine(LineCode cov_code);
Coverage allCoverage(Coverage cov, char* filename, char* input);
void sys(char* cmd);

#endif
