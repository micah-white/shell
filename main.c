#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char* wstrim(char* s);
void error();
char* substr(char* str, int index, int);
void split(char* str, char**, int*, int);
void printArray(char** a, int s);
int contains(char** arr, char* token, int);

extern char **environ;

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
	char* ampersand = (char*) malloc(sizeof(char)*2);
	char* redirect = (char*) malloc(sizeof(char)*2);
	strcpy(ampersand, "&");
	strcpy(redirect, ">");
	while(!feof(inputFile)){
		if(inputFile == stdin)
			printf("shell> ");
		getline(&s, &size, inputFile);
		if(feof(inputFile)){
			break;
		}

		s = wstrim(s);
		
		int tokenCount;
		split(s, tokens, &tokenCount, numSlots);
		
		int containsAmpersand = contains(tokens, ampersand, tokenCount);
		int startToken = 0;
		int endToken = tokenCount-1;
		int* pids = malloc(sizeof(int));
		int numCommands = 0;
		int wstatus;
		int quit = 0;
		int errorFound = 0;
		//pipe
		while(startToken != tokenCount && !errorFound){
			if(strlen(tokens[startToken]) == 0)
				break;
			if(containsAmpersand != -1){
				endToken = containsAmpersand-1;
			}
			int redirected = 0;
			char* outputFileName = NULL;
			if(contains(&tokens[startToken], redirect, endToken-startToken+1) > -1){
				outputFileName = tokens[endToken];
				redirected = 1;
				endToken -= 2;
			}

			//built-in commands
			if(strcmp(tokens[startToken], "exit") == 0){
				quit = 1;
				break;
			}
			else if(strcmp(tokens[startToken], "cd") == 0){
				if(endToken-startToken != 1){
					error();
					break;
				}
				if(chdir(tokens[1]) == -1)
					error();
			}
			else if(strcmp(tokens[startToken], "path") == 0){
				int numArgs = endToken - startToken;
				for(int i = 0;i < numPaths; i++)
					free(paths[i]);
				numPaths = numArgs;
				paths = (char**) realloc(paths, sizeof(char*) * numArgs);
				for(int i = 0; i < numArgs; i++){
					paths[i] = (char*) malloc(sizeof(char) * strlen(tokens[i+startToken+1]) + 1);
					strcpy(paths[i], tokens[i+startToken+1]);
				}
			}
			else if(strcmp(tokens[startToken], "env") == 0){
				int numArgs = endToken - startToken;
				if(numArgs == 0){ // print all environment variables
					char* temp_str = *environ;
					for(int i = 1; temp_str; i++){
						printf("%s\n", temp_str);
						temp_str = *(environ+i);
					}
				}
				else if(numArgs == 1){
					int r = putenv(tokens[startToken+1]);
					if(r != 0)
						error();
				}
				else if(strcmp(tokens[startToken+1], "-d") == 0 && numArgs == 2){
					int r = unsetenv(tokens[startToken+2]);
					if(r != 0)
						error();
				}
				else{
					error();
				}
			}
			else {
				//loops thru all paths to find command and breaks out after finding it
				int found = 0;
				
				for(int i = 0; i < numPaths; i++){
					char* filepath = malloc(sizeof(char)*strlen(paths[i]) + sizeof(char) * (strlen(tokens[startToken])+2));
					strcpy(filepath, paths[i]);
					strcat(filepath, "/");
					strcat(filepath, tokens[startToken]);
					if(access(filepath, X_OK) == 0){
						numCommands++;
						found = 1;
						//setting up args
						int numArgs = endToken-startToken+1;
						
						//args freed because it only copies pointers from tokens
						char* args[numArgs+1];
						args[0] = tokens[startToken];
						for(int j = 1; j < numArgs; j++){
							args[j] = tokens[j+startToken];
						}
						args[numArgs] = NULL;

						int pid = fork();
						if(pid == -1){
							fprintf(stderr, "failed to create child process\n");
							exit(1);
						}
						else if(pid == 0){ 	// child
							if(redirected == 1){
								int file = open(outputFileName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
								dup2(file, STDERR_FILENO);
								dup2(file, STDOUT_FILENO);
								close(file);
							}

							execv(filepath, args);
							exit(0);
						}
						else{				//parent
							if(containsAmpersand == -1){
								pids = realloc(pids,numCommands*sizeof(int*));
								pids[numCommands-1] = pid;
								waitpid((pid_t) pid, &wstatus, 0);
							}
							else{
								pids = realloc(pids,numCommands*sizeof(int*));
								pids[numCommands-1] = pid;
							}
						}
						
						free(filepath);
						
						break;
					}
					free(filepath);	
				}
				if(!found){
					error();
					errorFound = 1;
				}
			}
			if(containsAmpersand == -1){
				startToken = tokenCount;
			}
			else{
				startToken = containsAmpersand+1;
				endToken = tokenCount-1;
				int temp = contains(&tokens[startToken], ampersand, 1 + endToken - startToken);
				if(temp != -1)
					containsAmpersand = startToken + temp;
				else
					containsAmpersand = -1;
			}
		}
		for(int j = 0; j < numCommands; j++){
			waitpid((pid_t) pids[j], &wstatus, 0);
		}	
		free(pids);
		
		if(quit)
			break;		
	}
	free(ampersand);
	free(redirect);
	free(s);
	for(int i = 0; i < numSlots; i++)
		free(tokens[i]);
	free(tokens);
	for(int i = 0; i < numPaths; i++){
		free(paths[i]);
	}
	free(paths);
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
	printf("ws: %s\n", s);
	while(isspace(s[0])){
		shiftDown(s);
	}
	if(strlen(s) > 0){
		while(isspace(s[strlen(s)-1]))
			s[strlen(s)-1] = '\0';
	}
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
			fprintf(stderr, "tried to save more strings than there is space\n");
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
