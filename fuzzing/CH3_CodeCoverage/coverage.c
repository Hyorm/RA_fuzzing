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
#include<sys/stat.h>

#define BUFF_SIZE 1024

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
        char** org_code;
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

char* readfile(char* filename);
COVERAGE getCoverage(COVERAGE cov, char* filename, char* input); 
COMPCOV cmpCov(COVERAGE a, COVERAGE b);
void sys(char* cmd);
LINECODE line_parse(char* str);
COVLINE getCovLine(COVERAGE cov);

int main(int argc, char** argv){

	COVERAGE a = getCoverage(a, argv[1], "Hello%2c+world%21");
	COVERAGE b = getCoverage(b, argv[1], "Hello world");
	
	cmpCov(a, b);
}

COVLINE getCovLine(COVERAGE cov){

	COVLINE covline;
	int* covLine = malloc(sizeof(int)*(cov.cov_code.line));
	int lineIdx = 0;
	char** orgcov = (char**)malloc(sizeof(char*)*(cov.cov_code.line));
	for(int i = 0 ; i < cov.cov_code.line ; i++){
		
		orgcov[i] = malloc(sizeof(char)*sizeof(cov.cov_code.code[i]));
		strcpy(orgcov[i], cov.cov_code.code[i]);
		
		char* ptr; 
		ptr = strtok(orgcov[i], ":");
		//printf("ptr: %s\n", ptr);
		if((ptr !=NULL) && (strcmp(ptr, "    #####") != 0)){
			if((strcmp(ptr, "        -") == 0))
				covline.non_cover_line++;
			else{
				ptr = strtok(NULL, ":");
				if(ptr !=NULL){
					covLine[lineIdx++] = atoi(ptr);
					//printf("cov %s %d\n", ptr, covLine[lineIdx-1]);
				}
			}	
		}
	}

	covline.line = covLine;
	covline.idx = lineIdx;
	return covline;

}

LINECODE line_parse(char* str){

	char* ptr;
	char* tok;

	LINECODE linecode;

	char** str_after;	

	int tok_num = 0, str_idx = 0;
	
	tok = malloc(sizeof(char)*strlen(str));

	strcpy(tok, str);
	
	ptr = strtok(tok, "\n");
	while(ptr != NULL){

		tok_num++;
		ptr = strtok(NULL, "\n");

	}

	str_after = (char**)malloc(sizeof(char*)*(tok_num+2));

	free(tok);

	tok = malloc(sizeof(char)*strlen(str));

	strcpy(tok, str);

	ptr =  strtok(tok, "\n");

	 while(ptr != NULL){

		str_after[str_idx] = malloc(sizeof(char)*(strlen(ptr)+1));

		strcpy(str_after[str_idx++], ptr);
	
		ptr = strtok(NULL, "\n");

	}

	linecode.code = str_after;
	linecode.line = str_idx;
	return linecode;

}

void sys(char* cmd){

	if(system(cmd) < 0){
		printf("system error\n");
		exit(1);
	}

	free(cmd);

}

COVERAGE getCoverage(COVERAGE cov, char* filename, char* input){

	COVLINE covline;
	cov.filename = filename;
	cov.input = input;
	cov.cov_code = line_parse(readfile(filename));

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

	cov.cov_code = line_parse(readfile(cmd));
	cov.cov_lines = getCovLine(cov);

	if(cov.cov_lines.idx != 0)
		cov.coverage = (float)cov.cov_lines.idx/(float)(cov.cov_code.line - cov.cov_lines.non_cover_line);
	else 
		cov.coverage = 0.0;
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

char* readfile(char* filename){

	FILE* fp;
	int filesize = 0;

	char* buffer;	
	
	if(0<(fp = fopen(filename, "r"))){	
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		buffer = malloc(filesize+1);
		memset(buffer, 0, filesize + 1);

		fseek(fp, 0, SEEK_SET);

		fread(buffer, filesize, 1, fp);

		fclose(fp);
	}
	else{
	}
	return buffer;

}
