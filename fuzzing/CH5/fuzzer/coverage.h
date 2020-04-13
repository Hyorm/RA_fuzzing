#ifndef __COVERAGE_H__
#define __COVERAGE_H__

typedef struct _LineCode{
	
	char** code;
	int line;

}LineCode;

typedef struct _CoverLine{

	int* line;
	int idx;
	int non_cover_line;

}CoverLine;

typedef struct _Coverage{

        char* filename;
        CoverLine cov_lines;
        LineCode cov_code;
        float coverage;
        char* input;

}Coverage;

typedef struct _CmpCoverage{

        CoverLine onlya;
        CoverLine onlyb;
        CoverLine intersecion;

}CmpCoverage;

LineCode readfile(char* filename);
Coverage getCoverage(Coverage cov, char* filename, char* input);
CmpCoverage getCmpCoverage(Coverage a, Coverage b);
CoverLine getCoverLine(Coverage cov);
Coverage allCoverage(Coverage cov, char* filename, char* input);
void sys(char* cmd);

#endif
