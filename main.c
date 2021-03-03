#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

char* wstrim(char* s);
void error();
char* substr(char* str, int index, int);
void split(char* str, char**, int*, int);
void printArray(char** a, int s);
int contains(char** arr, char* token, int);

void main(int argc, char* argv[]){
	if(argc >= 3){
		error();
	}

	FILE* inputFile = stdin;
	if(argc == 2)
		inputFile = fopen(argv[1], "r");

	if(inputFile == NULL){
		error();
		exit(1);
	}

	char* s = (char*) malloc(sizeof(char)*100);
	size_t size = sizeof(char)*100;
	int numSlots = 15;
	char** tokens = (char**) malloc(sizeof(char*) * numSlots);
	for(int i = 0; i < numSlots; i++){
		tokens[i] = (char*) malloc(sizeof(char) * 50);
		strcpy(tokens[i], "\0");
	}

	char** paths = malloc(sizeof(char*));
	paths[0] = malloc(sizeof("/bin"));
	strcpy(paths[0], "/bin");
	int numPaths = 1;
	char* ampersand;
	char* redirect;
	while(!feof(inputFile)){
		printf("shell> ");
		getline(&s, &size, inputFile);
		// printf("hey: \"%s\"\n", s);
		// printf("hello?\n");
		if(feof(inputFile) && strlen(s) == 0){
			printf("end\n");
			break;
		}
		// printf("hello?\n");
		FILE* outputFile = stdout;
		s = wstrim(s);
		int tokenCount;
		split(s, tokens, &tokenCount, numSlots);
		// printArray(tokens, tokenCount);

		ampersand = (char*) malloc(sizeof(char)*2);
		redirect = (char*) malloc(sizeof(char)*2);
		strcpy(ampersand, "&");
		strcpy(redirect, ">");
		int contained = contains(tokens, ampersand, tokenCount);
		int startToken = 0;
		int endToken = tokenCount-1;
		// char** command = tokens;
		// int tc = tokenCount;
		int* pids = malloc(sizeof(int));
		int numCommands = 0;
		int wstatus;
		int quit = 0;
		while(startToken != tokenCount){
			if(contained != -1){
				endToken = contained-1;
				// numCommands++;
			}
			//built-in commands
			// printf("token: %s\n", tokens[startToken]);
			if(strcmp(tokens[startToken], "exit") == 0){
				quit = 1;
				break;
			}
			else if(strcmp(tokens[startToken], "cd") == 0){

			}
			else if(strcmp(tokens[startToken], "path") == 0){

			}
			else if(strcmp(tokens[startToken], "env") == 0){

			}
			else {
				//loops thru all paths to find command and breaks out after finding it
				int found = 0;
				numCommands++;
				for(int i = 0; i < numPaths; i++){
					char* filepath = malloc(sizeof(paths[i]) + sizeof(tokens[0]) + 1);
					strcpy(filepath, paths[i]);
					strcat(filepath, "/");
					strcat(filepath, tokens[0]);
					// printf("filepath: %s\n", filepath);
					if(access(filepath, X_OK) == 0){
						found = 1;
						//setting up args
						int numArgs = endToken-startToken+1;
						// printf("numargs %d, startToken: %d, endToken %d\n", numArgs, startToken, endToken);
						char* args[numArgs+1];
						args[0] = tokens[0];
						for(int j = 1; j < numArgs; j++){
							args[j] = tokens[j+startToken];
						}
						args[numArgs] = NULL;
						// strcpy(args[numArgs], nil);
						// printArray(tokens, tokenCount);
						// printArray(&args, numArgs);

						int pid = fork();
						if(pid == -1){
							fprintf(stderr, "failed to create child process\n");
							exit(1);
						}
						else if(pid == 0){ // child
							printf("this is child %d\n", getpid());
							execv(filepath, args);
							exit(0);
						}
						else{
							if(contained == -1){
								startToken = tokenCount;
								waitpid((pid_t) pid, &wstatus, 0);
							}
							else{
								// printf("contained: %d, numTokens %d\n", contained,tokenCount);
								startToken = contained+1;
								endToken = tokenCount-1;
								
								int temp = contains(&tokens[startToken], ampersand, 1 + endToken - startToken);
								if(temp != -1)
									contained = startToken + temp;
								else
									contained = -1;
								// printf("new contained %d\n", contained);
								pids = realloc(pids,numCommands*sizeof(int*));
								pids[numCommands-1] = pid;
							}
							// for(int k = 0; k < numArgs + 1; k++)
							// 	free(args[k]);
							// free(args);
						}
						free(filepath);
						
						break;
					}
					free(filepath);	
				}
				if(!found){
					error();
					exit(1);
				}
			}
		}
		for(int j = 0; j < numCommands; j++)
			waitpid((pid_t) pids[j], &wstatus, 0);
		free(pids);
		// printf("parent done\n");
		if(quit)
			break;		
	}

	free(s);
	for(int i = 0; i < numSlots; i++)
		free(tokens[i]);
	free(tokens);
	for(int i = 0; i < numPaths; i++){
		free(paths[i]);
	}
	free(paths);
	free(ampersand);
	free(redirect);
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

int contains(char** arr, char* token, int s){
	for(int i = 0; i < s; i++){
		if(strcmp(arr[i], token) == 0){
			return i;
		}
	}
	return -1;
}
