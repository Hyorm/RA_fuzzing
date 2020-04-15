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
#include"greyboxRunner.h"
#include"greyboxFuzzer.h"

greyboxRunner run_greyboxRunner(greyboxRunner gr, char* filename, char** seed, int seed_size, int min_mtt, int max_mtt){

	GreyboxFuzzer gf = {fuzz_GreyboxFuzzer};

	struct timeval now;

        gettimeofday(&now, NULL);
        srand((unsigned int)now.tv_usec);

        int mutate_size = rand()%max_mtt+min_mtt;
	Input* input = malloc(sizeof(input)*(seed_size+mutate_size));
	for(int i = 0; i < seed_size+mutate_size; i++){
		input[i].input = malloc(sizeof(char)*MAX_SIZE);
		if(i<seed_size)
			memcpy(input[i].input, seed[i], strlen(seed[i])-1);
		else{
			int pop = 0;
			if(i > 1)pop = rand()%(i-1);
			char* candidate;
			while(1){
				candidate = gf.fuzz(input[pop].input);
				if(strlen(candidate)>2)break;
			}
			memcpy(input[i].input, candidate, strlen(candidate)-1);
		}
	}
	Coverage run_cov;

	run_cov = getCoverage(run_cov, filename, gf.fuzz(input[0].input));
	for(int i = 1; i < seed_size+mutate_size; i++){
		
		run_cov = allCoverage(run_cov, filename, gf.fuzz(input[i].input));
		input[i].value = run_cov.cov_line.value;
		printf("coverage: %2.2f\n", run_cov.coverage * 100);
	}
	Population tmp ={input, seed_size, run_cov}; 	
	gr.pop = tmp;
	return gr;
}

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
        char* cmd = malloc(sizeof(char)*(strlen(filename)+strlen("gcc -fprofile-arcs -ftest-coverage ") + 1));

        sprintf(cmd, "gcc -fprofile-arcs -ftest-coverage %s", filename);
        sys(cmd);

        cmd = malloc(sizeof(char)*(strlen(inp_)+strlen("./a.out  > _std_out")+1));
        sprintf(cmd, "./a.out %s > _std_out", inp_);
        //sys(cmd);
	int returncode = 0;

	returncode = system(cmd);

	if(returncode == 0)
		cov.cov_line.value = 1;
	else if((returncode < 0) || (returncode > 1)){
                cov.cov_line.value = 2;
	}
	else cov.cov_line.value = 3;

        if(strcmp(cmd, "") !=0){
                memset(cmd, 0x00, sizeof(cmd));
        }

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen("gcov   > _gcov_std_out") + 1));
        sprintf(cmd, "gcov %s  > _gcov_std_out", filename);
        sys(cmd);

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen(".gcov") + 1));
        sprintf(cmd, "%s.gcov", filename);

        LineCode cov_code = readfile(cmd);

        cov.cov_line = getCoverLine(cov_code);

	if(cov.cov_line.idx != 0)
                cov.coverage = (float)cov.cov_line.idx/(float)(cov_code.line - cov.cov_line.non_cover_line);
        else
                cov.coverage = 0.0;

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen("rm .gcov") + 1));
        sprintf(cmd, "rm %s.gcov", filename);
        //sys(cmd);

        return cov;

}

CmpCoverage getCmpCoverage(Coverage a, Coverage b){
	
	CmpCoverage comp;
	
	comp.onlya.line = malloc(sizeof(int)* a.cov_line.idx);
        comp.onlya.idx = 0;

        comp.onlyb.line = malloc(sizeof(int)* b.cov_line.idx);
        comp.onlyb.idx = 0;

        if(a.cov_line.idx > b.cov_line.idx)
                comp.intersecion.line = malloc(sizeof(int)* b.cov_line.idx);
        else
                comp.intersecion.line = malloc(sizeof(int)* a.cov_line.idx);
        comp.intersecion.idx = 0;

        for(int i = 0; i < a.cov_line.idx ; i++){
                int inflag = 0;
                for(int j = 0 ; j < b.cov_line.idx ; j++){
                        if(a.cov_line.line[i] == b.cov_line.line[j]){
                                inflag = 1;
                                break;
                        }
                }
                if(inflag == 1)comp.intersecion.line[comp.intersecion.idx++] = a.cov_line.line[i];
                else comp.onlya.line[comp.onlya.idx++] = a.cov_line.line[i];
        }

        for(int i = 0; i < b.cov_line.idx ; i++){
                int inflag = 0;
                for(int j = 0 ; j < a.cov_line.idx ; j++){
                        if(a.cov_line.line[j] == b.cov_line.line[i]){
                                inflag = 1;
                                break;
               		}
		}
                if(inflag == 1){
                	int othflag = 0;

                        for(int j = 0; j < comp.intersecion.idx ; j++){
				if(comp.intersecion.line[j]==b.cov_line.line[i])
                                	othflag++;
                        }

                        if(othflag == 0)comp.intersecion.line[comp.intersecion.idx++] = b.cov_line.line[i];
	        }
        	else comp.onlyb.line[comp.onlyb.idx++] = b.cov_line.line[i];
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

CoverLine getCoverLine(LineCode cov_code){

	CoverLine covline;

        int* covLine = malloc(sizeof(int)*(cov_code.line));
        int lineIdx = 0;
        covline.non_cover_line = 0;
        char orgcov[BUFF_SIZE];
        for(int i = 0 ; i < cov_code.line ; i++){
                if(strlen(cov_code.code[i]) != 0)strcpy(orgcov, cov_code.code[i]);
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

Coverage allCoverage(Coverage cov, char* filename, char* input){

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

	char* cmd = malloc(sizeof(char)*(strlen(inp_)+strlen("./a.out   > _std_out")+1));
        sprintf(cmd, "./a.out %s > _std_out", inp_);

	int returncode = 0;

	returncode = system(cmd);

        if(returncode == 0)
                cov.cov_line.value = 1;
        else if((returncode < 0) || (returncode > 1)){
		cov.cov_line.value = 2;
	}
        else cov.cov_line.value = 3;                                               

        if(strcmp(cmd, "") !=0){
                memset(cmd, 0x00, sizeof(cmd));
        }	

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen("gcov   > _gcov_std_out") + 1));
        sprintf(cmd, "gcov %s  > _gcov_std_out", filename);
        sys(cmd);

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen(".gcov") + 1));
        sprintf(cmd, "%s.gcov", filename);

        LineCode cov_code = readfile(cmd);

        cov.cov_line = getCoverLine(cov_code);

        if(cov.cov_line.idx != 0)
                cov.coverage = (float)cov.cov_line.idx/(float)(cov_code.line - cov.cov_line.non_cover_line);
        else
                cov.coverage = 0.0;

        cmd = malloc(sizeof(char)*(strlen(filename)+strlen("rm .gcov") + 1));
        sprintf(cmd, "rm %s.gcov", filename);
        //sys(cmd);

        return cov;

}

void sys(char* cmd){

	if(system(cmd) < 0){
                printf("system error\n");
                exit(1);                                                                                 }
        if(strcmp(cmd, "") !=0){
                memset(cmd, 0x00, sizeof(cmd));
        }

}
