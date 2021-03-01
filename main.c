#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* wstrim(char* s);
void error();
char* substr(char* str, int index, int);
void split(char* str, char**, int*, int);
void printArray(char** a, int s);

void main(int argc, char* argv[]){
	if(argc >= 3){
		error();
	}

	FILE* inputFile = stdin;
	if(argc == 2)
		inputFile = fopen(argv[1], "r");

	char* s = (char*) malloc(sizeof(char)*100);
	size_t size = sizeof(char)*100;
	int numSlots = 15;
	char** tokens = (char**) malloc(sizeof(char*) * numSlots);
	for(int i = 0; i < numSlots; i++){
		tokens[i] = (char*) malloc(sizeof(char) * 50);
	}
	while(!feof(inputFile)){
		printf("shell> ");
		getline(&s, &size, inputFile);
		// strcpy(s, "this is a test");
		if(feof(inputFile)){
			printf("\n");
			break;
		}
		FILE* outputFile = stdout;
		s = wstrim(s);
		int arrCount;
		split(s, tokens, &arrCount, numSlots);
		printArray(tokens, arrCount);
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
	for(int i = 0; i < numSlots; i++)
		free(tokens[i]);
	free(tokens);
	if(argc == 2)
		fclose(inputFile);
	exit(0);
}

void shiftDown(char* s){
	int i = 0;
	while(s[i] != '\0'){;
		s[i] = s[i+1];
		i++;
	}
}

char* wstrim(char* s){
	while(isspace(s[0])){
		shiftDown(s);
	}
	while(isspace(s[strlen(s)-1]))
		s[strlen(s)-1] = '\0';
	return s;
}

void error(){
	fprintf(stderr, "An error has occurred\n");
	exit(1);
}

void split(char* str, char** arr, int* c, int s){
	int index = 0;
	int count = 0;
	for(int i = 0; i <= strlen(str); i++){
		if(i == strlen(str) || str[i] == ' '){
			if(i == index+1){
				index++;
			}
			else{
				char* sub = wstrim(substr(str, index, i-index));
				printf("sub: \"%s\"\n", sub);
				strcpy(arr[count++], sub);
				free(sub);
				index = i;
			}
		}
		if(str[i] == '&' || str[i] == '>'){
			char temp[2];
			temp[0] = str[i];
			temp[1] = '\0';
			char* sub = wstrim(substr(str, index, i-index));			
			if(i == index+1){
				strcpy(arr[count++], temp);
				index = ++i;
			}
			else{
				strcpy(arr[count++], sub);
				strcpy(arr[count++], temp);
				index = ++i;
			}
			free(sub);
		}
		else if( count > s){
			printf("tried to save more strings than there is space\n");
			return;
		}
	}
	if(index != strlen(str)){
		fprintf(stderr, "didn't get all the args\n");
	}

	*c = count;
}

char* substr(char* str, int start, int length){
	int i = 0;
	char* r = (char*) malloc(sizeof(char) * length + 1);
	if(strlen(str) == 0 || (start + length-1) > strlen(str)){
		fprintf(stderr, "tried substring of empty string or index above length");
		return NULL;
	}

	while(str[i+start] != '\0' && length != 0){
		r[i] = str[i+start];
		i++;
		length--;
	}

	r[i] = '\0';

	return r;
}

void printArray(char** a, int s){
	for(int i = 0; i < s; i++){
		printf("\"%s\"\n", a[i]);
	}
}

