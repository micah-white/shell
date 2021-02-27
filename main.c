#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* wstrim(char* s);
void error();
void substr(char* str, int index);
char** split(char* str);

void main(int argc, char* argv[]){
	if(argc >= 3){
		error();
	}

	char* a = malloc(sizeof(char)*100);
	strcpy(a, "this is a test");
	substr(a,3);
	printf("%s\n", a);

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
			break;
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

	free(s);
	if(argc == 2)
		fclose(inputFile);
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

char** split(char* str){
	char** arr = (char**) malloc(sizeof(char*) * 10);
	for(int i = 0; i < 10; i++){
		arr[i] = (char*) malloc(sizeof(char) * 50);
	}

	int size = 0;
	char* og;
	char delimit = ' ';

	while(str != NULL){
		og = strsep(&str, &delimit);
		if(str != NULL){

		}
	}

	return NULL;
}

void substr(char* str, int index){
	int i = 0;
	if(strlen(str) == 0 || index > strlen(str)){
		fprintf(stderr, "tried substring of empty string or index above length");
		return;
	}

	while(str[i+index] != '\0'){
		str[i] = str[i+index];
		i++;
	}

	str[i] = '\0';
}