#include <stdio.h>
#include <stdio.h>
#include <string.h>
void exception(void);

int main(int argc, char** argv){

	if((strlen(argv[1])>0)&& (argv[1][0] == 'b')){
		if((strlen(argv[1])>0)&& (argv[1][1] == 'a')){
			if((strlen(argv[1])>0)&& (argv[1][2] == 'd')){
				if((strlen(argv[1])>0)&& (argv[1][3] == '!')){
					exception();
				}
			}
		}
	}
		
}	

void exception(void){

	printf("exception!");

}
