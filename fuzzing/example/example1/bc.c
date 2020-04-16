#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv){

	if(argc > 2)
		printf("%d\n",atoi(argv[1])+atoi(argv[2]));
	else
		fprintf(stderr, "parsing error\n");
}
