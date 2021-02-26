#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* wstrim(char* s);
void error();

void main(int argc, char* argv[]){
	if(argc >= 3){
		error();
	}

	FILE* inputFile = stdin;
	if(argc == 2)
		inputFile = fopen(argv[1], "r");

	char* s = (char*) malloc(sizeof(char)*100);
	size_t size = sizeof(char)*100;
	while(!feof(inputFile)){
		printf("shell> ");
		getline(&s, &size, inputFile);
		if(feof(inputFile)){
			printf("\n");
			exit(0);
		}
		FILE* outputFile = stdout;
		s = wstrim(s);

		//built-in commands
		if(strcmp(s, "exit") == 0){
			break;
		}
		else if(strcmp(s, "cd") == 0){

		}
		else if(strcmp(s, "path") == 0){

		}
		else if(strcmp(s, "env") == 0){

		}
		else {

		}
	}

	exit(0);
}

char* wstrim(char* s){
	while(isspace(s[0])){
		s = &(s[1]);
	}
	while(isspace(s[strlen(s)-1]))
		s[strlen(s)-1] = '\0';
	return s;
}

void error(){
	fprintf(stderr, "An error has occurred\n");
	exit(1);
}

// char** split