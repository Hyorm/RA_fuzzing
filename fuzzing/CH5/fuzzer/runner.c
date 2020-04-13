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
        else{                                                                                                  printf("file open error\n");                                                                   rrp.program_output = "file open error";
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
        else if(rrp.returncode < 0 | rrp.returncode == 65280)
                rr.value = prnr.prgRnr.fail;
        else
                rr.value = prnr.prgRnr.unresolved;

        rr.program_output = rrp.program_output;
        rr.program_error = rrp.program_error;

        return rr;

}
