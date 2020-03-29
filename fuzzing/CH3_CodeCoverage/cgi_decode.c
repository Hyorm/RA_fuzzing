#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
char* cgi_decode(char* str){

	char hex_values[16][2] = {
		{'0', 0},{ '1', 1},{ '2', 2},{ '3', 3},{ '4', 4},
		{'5', 5},{ '6', 6},{ '7', 7},{ '8', 8},{ '9', 9},
		{'a', 10},{ 'b', 11},{ 'c', 12},{ 'd', 13},{ 'e', 14},{ 'f', 15},
		{'A', 10},{ 'B', 11},{ 'C', 12},{ 'D', 13},{ 'E', 14},{ 'F', 15}
	};

	char* return_str = malloc(sizeof(char)*strlen(str));
	
	int return_str_num = 0;

	for(int i = 0; i < strlen(str); i++){
		
		char char_str = str[i];

		if(char_str == '+')return_str[return_str_num++] = ' ';
		else if(char_str == '%'){
		
			char digit_high = str[i+1], digit_low= str[i+2];
			
			i += 2;

			int dh = -1;
			int dl = -1;
			for(int j = 0; j < 16 ; j++ ){
				if(digit_high == hex_values[j][0]){
					dh = j;
					break;
				}
			}
			for(int j = 0; j < 16 ; j++ ){
				if(digit_low == hex_values[j][0]){ 
					dl = j;
					break;
				}
			}
			if((dh <0) || (dl < 0)){
				printf("Invalid encoding\n");
				exit(1);
			}

			return_str[return_str_num++] = (int)hex_values[dh][1] * 16 + (int)hex_values[dl][1];
		
		}
		else return_str[return_str_num++] = char_str;
	
	}
	return_str[return_str_num] = '\0';

	return return_str;
}


int main(int argc, char** argv){

	printf("%s\n", cgi_decode("Hello\%2c+world%21"));

	//Black-Box Testing
	assert(strcmp(cgi_decode("+"), " ") == 0);
	assert(strcmp(cgi_decode("\%20")," ") == 0);
	assert(strcmp(cgi_decode("abc"), "abc") == 0);
	cgi_decode("%?a");

}
