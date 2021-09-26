#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h> 

typedef struct Buffer Buffer;

// ArrayList architecture
typedef struct Buffer {
    	int size, cap;
	char ** arr;
} Buffer;

// Data structure prototypes
Buffer * createBuffer();
void appendCommand(Buffer *, char *);
void expand(Buffer *);

// Helper function prototypes
void printCommands(Buffer *);
int doesDirectoryExist(char *, struct stat);
void printString(char *);

// Shell command prototypes
void whereami(char *);
void start(char *);

int main() {
	// Buffer to store recent commands
	Buffer * commandBuffer;
	commandBuffer = createBuffer();
	
	// Holds metadata about directories
	struct stat s;
	
	// Path to the current working directory
	char * currentdir = getcwd(currentdir, 100);

	start("/usr/bin/vim");


	return 0;
}

// Starts a program with or without parameters
// TODO: demlimit command string and place in args array
void start(char * command){
	int status;
	char * args[] = {command, NULL};

	pid_t pid = fork();
	if(pid == -1) {
		printf("Error forking");
	} else if(pid == 0) {
		execv(args[0], args);
	} else {
		wait(&status);
	}
}

// Prints to the terminal current directory
void whereami(char * currentdir) {
	printString(currentdir);
}

// Create the buffer to store the commands
Buffer * createBuffer() {
	Buffer * ret = calloc(1, sizeof(Buffer));
	ret->size = 0;
	ret->cap = 8;
	ret->arr = calloc(ret->cap, sizeof(char *));
	return ret;
}

// Append command to Buffer
void appendCommand(Buffer * buffer, char * command) {
    // Location where command will be appended
	int index = buffer->size;

	// If buffer reaches capacity, expand size of buffer
	if(index >= buffer->cap) {
		expand(buffer);
	}
	// Append command, increment size by 1
	buffer->arr[index] = command;
	buffer->size++;
}

// Reallocate memory by a multiple of two
void expand(Buffer * buffer) {
    buffer->cap = buffer->cap * 2;
	char ** largerArr = realloc(buffer->arr, buffer->cap * sizeof(char *));
	buffer->arr = largerArr;
}

// Print the Commands 
void printCommands(Buffer * buffer) {
	for(int i = 0; i < buffer->size; i++) {
		printString(buffer->arr[i]);
	}
}

// Returns 1 if directory exist, returns 0 if does NOT exist
int doesDirectoryExist(char * directory, struct stat s) {
	int status = stat(directory, &s);
	if(status == -1) {
		return 0;
	} else {
		return 1;
	}
}

// Print a string
void printString(char * str) {
	printf("%s", str);
	printf("\n");
}

