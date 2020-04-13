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
#include"coverage.h"

LineCode readfile(char* filename){

	LineCode linecode;
	FILE* fp;

	char buff[BUFF_SIZE];
	
	int linenum = 0;
	int max_size = 0;

	if(0 < (fp = fopen(filename, "r"))){

		while(!feof(fp)){

			fgets(buff, BUFF_SIZE-1, fp);
			linenum++;
			if(strlen(buff) > max_size)
				max_size = strlen(buff);
		}
		fclose(fp);
	}
	else{

		printf("Error: FILE Open Error");
		exit(1);
	}

	char** buffer = malloc(linenum * sizeof(char*));

	for(int i = 0; i< linenum ; i++){
		buffer[i] = (char*)malloc(sizeof(char)*max_size);
	}

	if(0<(fp = fopen(filename, "r"))){
		for(int i = 0; i< linenum ; i++){
			fgets(buffer[i], BUFF_SIZE-1, fp);
		}
		fclose(fp);
	}
	else{

                printf("Error: FILE Open Error");
                exit(1);
        }

	linecode.code = buffer;
	linecode.line = linenum-1;

	return linecode;
	
}

Coverage getCoverage(Coverage cov, char* filename, char* input){
	
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

	cov.cov_lines = getCoverLine(cov);

	if(cov.cov_lines.idx != 0)
		cov.coverage = (float)cov.cov_lines.idx/(float)(cov.cov_code.line - cov.cov_lines.non_cover_line);
	else
		cov.coverage = 0.0;

	cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen("rm .gcov") + 1));	
	sprintf(cmd, "rm %s.gcov", cov.filename);
	//sys(cmd);

        return cov;

}

CmpCoverage getCmpCoverage(Coverage a, Coverage b){

	CmpCoverage comp;

	comp;
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
                        for(int j = 0; j < comp.intersecion.idx ; j++){                                                        if(comp.intersecion.line[j]==b.cov_lines.line[i])
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

CoverLine getCoverLine(Coverage cov){
	CoverLine covline;

	printf("malloc in getCmpCoverage\n");
	int* covLine = malloc(sizeof(int)*(cov.cov_code.line));
	int lineIdx = 0;

	covline.non_cover_line = 0;
        char orgcov[BUFF_SIZE];
        for(int i = 0 ; i < cov.cov_code.line ; i++){
                strcpy(orgcov, cov.cov_code.code[i]);
                char* ptr;                                                                                     ptr = strtok(orgcov, ":");
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

Coverage allCoverage(Coverage cov, char* filename, char* input){
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
        }                                                                                              inp_[invalid] = '\0';
        cov.input = inp_;

	char* cmd = malloc(sizeof(char)*(strlen(cov.input)+strlen("./a.out ")+1));
        sprintf(cmd, "./a.out %s", cov.input);
        sys(cmd);

        cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen("gcov ") + 1));
        sprintf(cmd, "gcov %s", cov.filename);

        sys(cmd);

        cmd = malloc(sizeof(char)*(strlen(cov.filename)+strlen(".gcov") + 1));
        sprintf(cmd, "%s.gcov", cov.filename);

        cov.filename = filename;

        cov.input = inp_;

        cov.cov_code = readfile(cmd);

        cov.cov_lines = getCoverLine(cov);

	if(cov.cov_lines.idx != 0)
		cov.coverage = (float)cov.cov_lines.idx/(float)(cov.cov_code.line - cov.cov_lines.non_cover_line);
	else
               	cov.coverage = 0.0;

        return cov;


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
