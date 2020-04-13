#ifndef __RUNNER_H__
#define __RUNNER_H__

#define BUFF_SIZE 1024

typedef struct _ReturnRunner{

        char* inp;
        char* value;

}ReturnRunner;

typedef struct _ReturnRunProcess{

        int returncode;
        char* value;
        char* program_output;
        char* program_error;

}ReturnRunProcess;

typedef struct _Runner{
        char* pass;// = "PASS";
        char* fail;// = "FAIL";
        char* unresolved;// = "UNRESOLVED";

        ReturnRunner (*run)(struct _Runner, char*);

}Runner;

typedef struct _PrintRunner{

        Runner prRnr;

        ReturnRunner (*run)(struct _PrintRunner, char*);

}PrintRunner;

typedef struct _ProgramRunner{

        Runner prgRnr;

        char* program;

        //Subprocess run
        ReturnRunProcess (*run_process)(struct _ProgramRunner, char*);

        ReturnRunProcess (*run)(struct _ProgramRunner, char*);

}ProgramRunner;

typedef struct _FunctionRunner{

        Runner fRnr;

        char* function;

}FunctionRunner;

typedef struct _FunctionCoverageRunner{

	Runner fcRnr;

}FunctionCoverageRunner;

ReturnRunner run_Runner(Runner rnr, char* inp);

ReturnRunner run_PrintRunner(PrintRunner pr, char* inp);

ReturnRunProcess run_process_Program(ProgramRunner prnr, char* inp);
ReturnRunProcess run_Program(ProgramRunner prnr, char* inp);

#endif
