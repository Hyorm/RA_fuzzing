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

int main(int argc, char** argv){
	Population pop;
	greyboxRunner gr = {pop, run_greyboxRunner};
	
	char** seed_input= (char**)malloc(sizeof(char*)*1);
        seed_input[0]= "http://www.google.com/search?q=fuzzing";	

	printf("in\n");
	gr = gr.run(gr, argv[1], seed_input, 1, 10, 100);

	printf("%f\n", gr.pop.cov.coverage);

}
