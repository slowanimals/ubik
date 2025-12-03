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

#define SHELL_BUFSIZE 1024
char *read_line(void){
	int bufsize = SHELL_BUFSIZE;
	char *buffer = malloc(size_of(char) * bufsize);
	int position = 0;
	int c;
	
	if (!buffer){
		fprintf(stderr,"shell: error allocating memory to buffer");
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
			bufsize += SHELL_BUFSIZE;
			buffer = realloc(buffer,bufsize);
		
			if (!buffer){
				fprintf(stderr,"shell: error allocating memory to buffer");
				exit (EXIT_FAILURE);
			}
		}
	}
}
