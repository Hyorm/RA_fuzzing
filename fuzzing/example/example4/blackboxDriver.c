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
#include"blackboxRunner.h"
#include"blackboxFuzzer.h"

int main(int argc, char** argv){
	Population pop;
	BlackboxRunner gr = {pop, run_BlackboxRunner};
	
	char** seed_input= (char**)malloc(sizeof(char*)*1);
        seed_input[0]= "good";	

	gr = gr.run(gr, argv[1], seed_input, 1, 0, 1000, 1);
//	for(int i = 0; i < gr.pop.size; i++)	
//		printf("%s\n", gr.pop.input[i].input);
	printf("%f\n", gr.pop.cov.coverage);

}
