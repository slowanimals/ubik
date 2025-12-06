# Shell (C)

Adapted from: https://brennan.io/2015/01/16/write-a-shell-in-c/  
My own notes for future reference (via Notion)  
Going to be adding more functionality, thinking of using it to help with AI development 

# Basic Functionality

1. **Initialize**: Reads and executes config files
2. **Interpret**: Reads and executes interactive commands or files
3. **Terminate**: Executes shutdown commands, frees memory, then terminates

```c
int main(int argc, char **argv){
	//load config files
	
	//run command loop
	shell_loop();
	
	//shutdown/terminate
	return EXIT_SUCCESS
}
```

# Basic Loop

1. **Read**: Read command line input
2. **Parse**: Separate command string into program and arguments
3. **Execute**: Run the parsed command

```c
void shell_loop(void){
	char *line; //* lets us allocate required space for the line
	char **args; //uses ** because it will hold the address of the args list
	int status;
	
	do{
		print("> ");
		line = read_line();
		args = split_line(line);
		status = execute(args);
		
		free(line)
		free(args)
	} while(status);
}
```

# Reading a line

- We do not know how much text a user will enter, so we start with a block and allocate more if needed
- Because EOF (end of file) is an int, we store each character as an int
- If we reach EOF (either is a newline or end) we terminate the current string and return it
    - Else, add the current character to the string
- Check if the next character will exceed our buffer size, we reallocate the buffer

### Dynamic Buffer

```c
int bufsize = SHELL_BUFSIZE; //constant prob
int position = 0;
char *buffer = (malloc(sizeof(char) * bufsize);
int c;

if(!buffer){
	fprintf(stderr, "shell: memory allocation error");
	exit(EXIT_FAILURE);
}
```

- `BUFSIZE` is a constant that would probably be 1024 (can store up to 1024 chars)
- `position` tracks the index of where the next character will be placed and how much of the buffer is filled
- `buffer` is a pointer that stores the starting address of the memory block
    - It asks the OS for a block of memory large enough to hold `bufsize` number of chars
    - We used **`malloc`** instead of `calloc` because calloc initializes each element to 0, which is unnecessary for this since we will always be rewriting memory
- `if(!buffer)` is just to ensure there aren’t any crashes if the system we are using isn’t allocating memory properly

### Populate Buffer

```c
while(1){
	c = getchar();
	if (c == EOF || c == '\n'){
		buffer[position] = '\0';
		return buffer;
	} else {
		buffer[position] = c;
	}
	position++;
}
```

- `getchar()` fetches next character in input
- `\0` represents a null character
- `while(1)` runs buffer population infinitely until we hit a newline/EOF

### Reallocate Memory

```c
if (position >= bufsize){
	bufsize += SHELL_BUFSIZE;
	buffer = realloc(buffer,bufsize);
	if (!buffer){
		fprintf(stderr,"shell: error reallocating memory");
		exit(EXIT_FAILURE);
}
```

- `bufsize` is expanded by adding the constant to it
- `realloc` is used to expand the existing memory block
    - It takes a pointer (`buffer`) and a new size (`bufsize`) and expands the memory
    - If the space immediately following is not free, it will change addresses to a different free space, so it has to be reassigned to the original pointer `buffer` to avoid any future issues
- We once again check for any issues to prevent any crashes

# Parsing a Line

- We can use whitespace to separate args from each other
- We can use the library `strtok` to tokenize the string with whitespace as the delimiter
    - `char *strtok(char *str, const char *delim)`
    - `strtok(NULL,delim)` tells the program to move to the next split character instead of starting over
- The function is mostly similar to reading

### Populate Buffer

```c
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
```

- We use `**` to indicate that we want to store a list of addresses
    - We don’t know what the size of each token will be, so we will just store a list of each token’s address
- We do `sizeof(char*)` to get the size of a char address

### Storing Tokens

```c
token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
```

- We use `strtok` before the while loop to split up the line and get the first item of token
- We do `realloc(tokens, bufsize * sizeof(char*)` instead of `realloc(tokens,bufsize)` because the size of a `char` address is > 1 byte
    - In the read line function, we just do `realloc(tokens, bufsize)` because the size of char is 1 byte so we can just enter the buffer size
- We do `strtok(NULL,delim)` to make token equal to the next item in the split list

# Start Process

- There are 2 ways of starting processes in Unix
    1. **Init:** Init is a program that runs when a Unix kernel is loaded, and it runs the whole time a computer is on. It manages running the rest of the processes that we will use
    2. **Fork**: 
        - `fork()` is a system call
        - When called, it makes a duplicate of the process and has both run
            - Original process is the **parent**, the duplicate is the **child**
                - Fork returns 0 to the child and the **process ID (PID)** to the parent of its child
        - The only way for any process to start is by an existing one duplicating itself
        - `exec()` is used to replace a currently running program with an new one
            - Exec stops the process running in **child**, loads up a new program, then starts a new process in its place
        - `wait()` can be used by the parent process to keep a tab on its children

### Initializing/Child

```c
int lsh_launch(char **args){
	pid_t pid, wpid;
	int status;
	
	pid = fork();
	if(pid == 0){
		if(execvp(args[0],args) == -1){
			perror("shell");
		}
		exit(EXIT_FAILURE);
	
	}
	....
```

- We initialize a parent id `pid` and work id `wpid`
- The child process takes the first condition, where `pid == 0`
- `execvp(program,array)` is a variant of the `exec` system call
    - Different variants take in different parameters
    - This is focused on having a **vector ‘v’** (array) and a **name ‘p,’** meaning the OS searches for the program in the path (many times we have to specify the path
- Exec will try to run the file with given command, but if it returns -1, we print an error message and exit
- `pid` will hold the value of the child process
- `wpid` will hold the PID of the process that completed
- There will be 2 processes, one where `pid`= 0 (child) and one where `pid` = *some number,* meaning it is the main process

### Fork Error

```c
	} else if (pid < 0) {
		perror("shell");
	}
...
```

- Checks if fork had an error, doesn’t exit

### Fork Success Handling

```c
	} else {
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1
} //end
```

- Parent process lands here
- Since child executes process, parent waits until it is done running
- `waitpid()` is called by the parent process to track state changes a specified child process and retrieve information about the child’s termination status
    - **`pid_t waitpid(pid_t pid, int *status, int options);`**
    - `&status` is a pointer to an int which the kernel stores exit status information of a terminated child. It is is the output parameter for `waitpid()`
    - `WUNTRACED` (options) is a flag that tells the function to return also for children that have been stopped
- A process’ state can change in a variety of ways, so we use the **macros** to wait until the processes are exited or killed
    - `WIFEXITED` returns **true** if child process returned terminated normally via `exit()`
    - `WIFSIGNALED` returns **true** if child process was terminated by a signal that wasn’t caught, such as Ctrl+C, segfault, kill, etc.
        - Status will be a bitfield that these macros scan through
- Function then returns 1 to call the function to ask for input again

# Shell Builtins

- Most commands a shell executes are programs, but not all
- For example cd cannot be its own program, else it would change its own directory then exit
    - The shell process itself must execute cd so that it can change its own directory and then have child processes inherit the new directory
- If there was a separate program called exit, it wouldn’t be able to exit the shell that called it
- There are also built in configuration scripts that change how the shell operates

### Declarations

```c
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
```

- It’s often important to do forward declarations (usually in .h files) to help avoid dependency issues when compiling

### Pre-defining Command Names

```c
char *builtin_str[]{
	"cd",
	"help",
	"exit"
};
int (*builtin_func[])(char**){
	&shell_cd;
	&shell_help;
	&shell_exit;
};
int shell_num_builtins(){
	return (sizeof(builtin_func) / sizeof(char *));
}
```

- `builtin_str` is an array of pointers which is why its declared so
- `(*builtin_func[])` breakdown
    - `builtin_func` is name of variable
    - `[]` specifies that its an array
    - `(*___)` the asterisk and surrounding parentheses indicates that it is an array of pointers
        - A lack of parentheses would just the address, but with it the command will get dereferenced
    - `char**` indicates that function takes in an array of strings, it is shorthand for:
        - `char **args` (theres literally no difference)
- To calculate length, we get the size of the entire array then divide it by the size of a char memory address

### Implementations

```c
int shell_cd(char **args){
	if (args[1] == NULL){
		fprintf(stderr, "Expected argument after \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0){
			perror("shell: cd failure");
		}
	}
	return 1;
}

int shell_help(char **args){
	//custom message
}

int shell_exit(char **args){
	return 0;
}
```

- `chdir` takes in a path and returns 0 if successful, which is why we check that
- `shell_exit` just returns 0 to terminate the process, as our shell runs while status == 1

# Lastly, Execute

- Function that launches a process or a builtin
- Checks if command equals a builtin, and if not it calls the launch function

```c
int shell_execute(char **args){
	if(args[0] == NULL){
		return 1;
	}
	for (i = 0; i < shell_num_builtins(){
		if (strcmp(args[0],builtin_str[i]) == 0){
			return (*builtin_func[i])(args);
	}
	return lsh_launch(args);
}
```
