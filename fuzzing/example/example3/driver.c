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
#include"mutationRunner.h"
#include"mutationFuzzer.h"

int main(int argc, char** argv){
	Population pop;
	MutationRunner gr = {pop, run_MutationRunner};
	
	char** seed_input= (char**)malloc(sizeof(char*)*1);
        seed_input[0]= "http://www.google.com/search?q=fuzzing";	

	gr = gr.run(gr, argv[1], seed_input, 1, 0, 10000, 1);
	//for(int i = 0; i < gr.pop.size; i++)	
	//	printf("%s\n", gr.pop.input[i].input);
	printf("%f\n", gr.pop.cov.coverage);

}
