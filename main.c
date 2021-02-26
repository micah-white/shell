#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* wstrim(char* s);

void main(int argc, char* argv[]){
	if(argc >= 3){
		printf("shell accepts 0 or 1 arguments\n");
		exit(1);
	}

	FILE* fp = stdin;
	if(argc == 2)
		fp = fopen(argv[1], "r");

	char* s = (char*) malloc(sizeof(char)*100);
	size_t size = sizeof(char)*100;
	while(!feof(fp)){
		printf("shell> ");
		getline(&s, &size, fp);
		if(feof(fp)){
			printf("\n");
			exit(0);
		}
		s = wstrim(s);
		if(strcmp(s, "exit") == 0){
			break;
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