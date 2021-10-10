#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
void appendCommandToBuffer(Buffer *, char *);
void expandBuffer(Buffer *);
char** createArgsArray(int);

// Helper function prototypes
void printCommands(Buffer *);
int doesDirectoryExist(char *, struct stat);
void printString(char *);
char** commandDelimiter(char []);
int countArgs(char []);

// Shell command prototypes
void whereami(char *);
void start(char *);

int main() {
	// Buffer to store recent commands
	Buffer * commandBuffer = createBuffer();
	// Path to the current working directory
	char * currentdir = getcwd(currentdir, 100);
	// Holds metadata about directories
	struct stat s;

	start("/usr/bin/vim");

	return 0;
}

// Starts a program with or without parameters
void start(char * command){
	// pid uses this value behind the scenes
	int status;
	char ** usersArgs = commandDelimiter(command);
	pid_t pid = fork();

	if(pid == -1) {
		printf("Error forking");
	} else if(pid == 0) {
		// Jumps into new child process
		execv(usersArgs[0], usersArgs);
	} else {
		// Waits for child process to terminate before proceeding
		wait(&status);
	}
}

// Splits a command into an array of all of its args
char ** commandDelimiter(char command[]) {
	char delimeter [] = " ";
	char argument [50];
	int argsIndex = 0;
	int i = 0;
	
	// Pointer used to iterate through through command stopping at all spaces
	char * ptr = strtok(command, delimeter);
	int numberArgs = countArgs(command);
	char** usersArgs = createArgsArray(numberArgs);

	// Split string wherever there are spaces
	while(ptr != NULL) {
		// Extracting string
		while(*ptr != '\0'){
			argument[i++] = *ptr++;
		}
		strcpy(usersArgs[argsIndex++], argument);
		// Pointing to next string
		ptr = strtok(NULL, delimeter);
		i = 0;
		// Clearing out argument string
		while(argument[i] != '\0') {
			argument[i++] = '\0';
		}
	}
	// exec() requires NULL as last value of array
	usersArgs[argsIndex] = NULL;
	return usersArgs;
}

// Returns the number of args contained in command
int countArgs(char command[]) {
	char delimeter[] = " ";
	int argsCounter = 0;
	char** createArgsArray(int numberArgs);
	// Pointer used to iterate through through command, stopping at all spaces
	char * ptr = strtok(command, delimeter);	
	// Count number of strings in command	
	while(ptr != NULL) {
		while(*ptr != '\0'){
			ptr++;
		}
		argsCounter++;
		ptr = strtok(NULL, delimeter);
	}
	// +1 for NULL value at end of the array
	return (argsCounter+1);
}

// Function allocates memory for array of args
char** createArgsArray(int numberArgs) {
	char** argsArray = (char**)malloc(sizeof(char*));
	for(int i = 0; i < numberArgs; i++) {
		argsArray[i] = (char*)malloc(50*sizeof(char));
	}
	return argsArray;
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
void appendCommandToBuffer(Buffer * buffer, char * command) {
    // Location where command will be appended
	int index = buffer->size;

	// If buffer reaches capacity, expand size of buffer
	if(index >= buffer->cap) {
		expandBuffer(buffer);
	}
	// Append command, increment size by 1
	buffer->arr[index] = command;
	buffer->size++;
}

// Reallocate memory by a multiple of two
void expandBuffer(Buffer * buffer) {
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
