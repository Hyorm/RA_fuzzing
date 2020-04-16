#ifndef __BLACKBOXRUNNER_H__
#define __BLACKBOXRUNNER_H__

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

}CoverLine;

typedef struct _Coverage{

	float coverage;
	CoverLine cov_line;

}Coverage;

typedef struct _Input{
	char* input;
	int value;
        float energy;
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

typedef struct _BlackboxRunner{

	Population pop;
	
	struct _BlackboxRunner (*run)(struct _BlackboxRunner, char*, char**, int, int, int, int);	

}BlackboxRunner;

struct _BlackboxRunner run_BlackboxRunner(BlackboxRunner gr, char* filename, char** seed, int seed_size, int min_mtt, int max_mtt, int flag);

Input* normalizedEnergy(Input* input, int size);
LineCode readfile(char* filename);
Coverage getCoverage(Coverage cov, char* filename, char* input);
CmpCoverage getCmpCoverage(Coverage a, Coverage b);
CoverLine getCoverLine(LineCode cov_code);
Coverage allCoverage(Coverage cov, char* filename, char* input);
void sys(char* cmd);

#endif
