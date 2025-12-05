#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



#define LINE_BUFSIZE 1024
char *read_line(void){
	int bufsize = LINE_BUFSIZE;
	char *buffer = malloc(sizeof(char) * bufsize);
	int position = 0;
	int c;
	
	if (!buffer){
		fprintf(stderr,"shell: error allocating memory to buffer\n");
		exit (EXIT_FAILURE);
	}
	while(1){
		c  = getchar();
		if (c == EOF || c == '\n'){
			buffer[position] = '\0';	
			return buffer;
		}else{
			buffer[position] = c;
		}	
		position++;

		if(position >= bufsize){
			bufsize += LINE_BUFSIZE;
			buffer = realloc(buffer,bufsize);
		
			if (!buffer){
				fprintf(stderr,"shell: error allocating memory to buffer\n");
				exit (EXIT_FAILURE);
			}
		}
	}
}

#define TOK_SIZE 64
#define DELIM "\t\r\a\n"
char **split_line(char *line){
	int bufsize = TOK_SIZE;
	char **tokens = malloc(sizeof(char*) * bufsize);
	char *token;
	int pos = 0;

	if (!tokens){
		fprintf(stderr,"shell: error allocating memory for tokens");
		exit(EXIT_FAILURE);
	}
	
	token = strtok(line, DELIM);
	while(token != NULL){
		tokens[pos] = token;
		pos++;
		
		if(pos >= bufsize){
			bufsize += TOK_SIZE;
			tokens = realloc(tokens, sizeof(char*) * bufsize);
			if (!tokens){
				fprintf(stderr, "shell: error reallocating memory for tokens");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL,DELIM);
	} 
	tokens[pos] = NULL;
	return tokens;
}

int shell_launch(char **args){
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	
	if (pid == 0) {
		if(execvp(args[0],args) == -1){
			perror("shell: error executing file");
		}		
		exit(EXIT_FAILURE);	
	} else if (pid < 0){
		perror("shell: error forking");
	} else {
		do{
			wpid = waitpid(pid, &status, WUNTRACED);	
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;	
}


int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[])(char **args) = {
	&shell_cd,
	&shell_help,
	&shell_exit
};

int builtin_size(){
	return sizeof(builtin_str)/sizeof(char*);
}

//built in implementation
int shell_cd(char **args){
	if (args[1] == NULL){
		fprintf(stderr, "shell: Expected argument after \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0){
			perror("shell: cd failure");	
		}	
	}
	return 1;
}

int shell_help(char **args){
	printf("Kaush's Shell\n");
	printf("Created December 2025\n");
	printf("Built in commands:\n");
	
	for (int i = 0; i < builtin_size(); i++){
		printf(" %s\n", builtin_str[i]);
	}
	return 1;
}

int shell_exit(char **args){
	return 0;
}


void shell_loop(void){
	char *line;
	char **args;
	int status;

	do{
		printf("> ");	
		line = read_line();
		args = split_line(line);
		status = shell_execute(args);
		free(line);
		free(args);

	}while(status);
}


int main(int argc, char **argv){
	
	//shell_loop();

	return EXIT_SUCCESS;
}
