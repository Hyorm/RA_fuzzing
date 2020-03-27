#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>
#include<time.h>
#include<setjmp.h>
#include<string.h>
#include<ctype.h>
#define EPSILON 0.000001

double my_sqrt(double x);
double my_sqrt_with_log(double x);
double my_sqrt_checked(double x);
double my_sqrt_fixed(double x);

void assertEquals(double x, double y);
int sqrt_program(char* x);

int _err_num = 0;

int main(int argv, char** argc){

	srand(time(NULL));
	/*
	//TODO: Debuging a Function
	
	printf("\n%.20f\n",my_sqrt_with_log(atoi(argc[1])));
	*/

	/*	
	//TODO: Checking a Function
	
	printf("\n%.20f\n",my_sqrt(atoi(argc[1]))*my_sqrt(atoi(argc[1])));
	
	*/
	/*	
	//TODO: Automating Test Execution
	
	double result = my_sqrt(4);
	double expected_result = 2.0;
	
	if(result == expected_result)
		printf("Test passed\n");
	else
		printf("Test failed\n");

	assert(my_sqrt(4) == 2);
	assert((my_sqrt(4) - 2) < EPSILON);
	
	assertEquals(my_sqrt(4), 2);
	assertEquals(my_sqrt(9), 3);
	assertEquals(my_sqrt(100), 10);
	*/	

	/*If EPSILON size's change, then result change
	//TODO: Generating Tests
	clock_t start, end;
	start = clock();
	assertEquals(my_sqrt(2) * my_sqrt(2), 2);
	assertEquals(my_sqrt(3) * my_sqrt(3), 3);
	assertEquals(my_sqrt(42.11) * my_sqrt(42.11), 42.11);

	for(int i = 1; i<=1000; i++){		
		
		int num = 1 + rand();
		assertEquals(my_sqrt(num) * my_sqrt(num), num);

	}
	
	end = clock();
	printf("timer: %.10f\n",((double)(end - start) / CLOCKS_PER_SEC));
	*/

	/*
	//TODO: Run-Time Verification
	
	printf("%.20f\n", my_sqrt_checked(2.0));
	*/

	/*
	//TODO: System Input vs Function Input
	
	sqrt_program("4");
	sqrt_program("-1");
	sqrt_program("xyzzy");
	*/
	
	/*
	//TODO:The Limits of Testing 
	assert(my_sqrt_fixed(0)==0);
	my_sqrt_fixed(-1);
	*/
}

double my_sqrt(double x){

	double approx = 0.0;
	double guess = x / 2;
	
	while(approx != guess){
		approx = guess;
		guess = (approx + x / approx) / 2;
	}
	
	return approx;
}

double my_sqrt_with_log(double x){
	
	double approx = 0.0;
	double guess = x / 2; 

	while(approx != guess){
		printf("approx = %.20f\n", approx);
		approx = guess;
		guess = (approx + x / approx) / 2; 
	}
	return approx;
}

void assertEquals(double x, double y){

	//printf("fabsf(%.20f - %.20f): %.20f, EPSILON: %.20f\n", x, y, fabsf(x - y), EPSILON);
	assert(fabsf(x - y) <=EPSILON);

}

double my_sqrt_checked(double x){

	double root = my_sqrt(x);
	
	assertEquals(root * root, x);

	return root;
}

int sqrt_program(char* x){
	
	double this = atof(x);

	if(this == 0.0){
		for(int i = 0; i < strlen(x); i++){
		
			if(isdigit(x[i])==0)printf("Illegal Input\n");
			return 1;
		}
	}
	if(this < 0.0){
		printf("Illegal Number\n");
		return 2;
	}
	printf("The root of %.20f is %.20f\n", this,my_sqrt(this));
	return 0;
}
double my_sqrt_fixed(double x){

	assert(0.0 <= x);

	if(x == 0.0)return 0.0;
	
	return my_sqrt(x);
}

