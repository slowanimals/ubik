#include <stdio.h>


int main(int argc, char **argv){
	
	shell_loop();

	return EXIT_SUCCESS;
}

void shell_loop(void){
	char *line;
	char **args;
	int status;

	do{
		print('> ');	
		line = read_line();
		args = split_line(line);
		status = execute(args);
		free(line);
		free(args);

	}while(status);
}

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
def char **split_line(char *line){
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

