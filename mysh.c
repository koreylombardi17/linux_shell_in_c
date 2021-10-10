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
	char** arr;
} Buffer;

// Data structure prototypes
Buffer * createBuffer();
void appendCommandToBuffer(Buffer*, char*);
void expandBuffer(Buffer*);
char** createArgsArray(int, int);

// Helper function prototypes
void printCommands(Buffer*);
int doesDirectoryExist(char*, struct stat);
void printString(char*);
char** commandDelimiter(char*);
int countArgs(char*);
int countLongestArg(char*);

// Shell command prototypes
void whereami(char*);
void start(char*);
int background(char*);
int dalek(int);

int main() {
	// Buffer to store recent commands
	Buffer * commandBuffer = createBuffer();
	// Path to the current working directory
	char * currentdir = getcwd(currentdir, 100);
	// Holds metadata about directories
	struct stat s;

	start("/usr/bin/vim");

	// Dont forget to free memory
	
	return 0;
}

// Prints to the terminal current directory
// TODO: Make function return int
void whereami(char* currentdir) {
	printString(currentdir);
}

// Starts a program with or without parameters
// TODO: Make function return int
// TODO: Test function with multiple args
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

// TODO: Implement function
int background(char* command) {

}

// TODO: Implement function
int dalek(int pid){

}

// Splits a command into an array of all of its args
char** commandDelimiter(char* command) {
	char delimeter [] = " ";
	char argument [50];
	int argsIndex = 0;
	int i = 0;
	
	// Pointer used to iterate through through command stopping at all spaces
	char* ptr = strtok(command, delimeter);
	int numberArgs = countArgs(command);
	int maxArgLength = countLongestArg(command);
	char** usersArgs = createArgsArray(numberArgs, maxArgLength);

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
int countArgs(char* command) {
	char delimeter[] = " ";
	int argsCounter = 0;
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

// Returns the length of the longest arg
int countLongestArg(char* command) {
	char delimeter[] = " ";
	int maxArgLength = 0;
	int argLength = 0;
	// Pointer used to iterate through through command, stopping at all spaces
	char * ptr = strtok(command, delimeter);	
	// Count number of strings in command	
	while(ptr != NULL) {
		while(*ptr != '\0'){
			ptr++;
			argLength++;
		}
		if(argLength > maxArgLength) {
			maxArgLength = argLength;
		}
		argLength = 0;
		ptr = strtok(NULL, delimeter);
	}
	// +1 for NULL character value at end of string
	return (maxArgLength+1);
}

// Function allocates memory for array of args
char** createArgsArray(int numberArgs, int maxArgLength) {
	char** argsArray = (char**)malloc(sizeof(char*));
	for(int i = 0; i < numberArgs; i++) {
		argsArray[i] = (char*)malloc(maxArgLength*sizeof(char));
	}
	return argsArray;
}

// Create the buffer to store the commands
Buffer* createBuffer() {
	Buffer * ret = calloc(1, sizeof(Buffer));
	ret->size = 0;
	ret->cap = 8;
	ret->arr = calloc(ret->cap, sizeof(char *));
	return ret;
}

// Append command to Buffer
void appendCommandToBuffer(Buffer* buffer, char* command) {
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
void expandBuffer(Buffer* buffer) {
    buffer->cap = buffer->cap * 2;
	char** largerArr = realloc(buffer->arr, buffer->cap * sizeof(char *));
	buffer->arr = largerArr;
}

// Print the Commands 
void printCommands(Buffer* buffer) {
	for(int i = 0; i < buffer->size; i++) {
		printString(buffer->arr[i]);
	}
}

// Returns 1 if directory exist, returns 0 if does NOT exist
int doesDirectoryExist(char* directory, struct stat s) {
	int status = stat(directory, &s);
	if(status == -1) {
		return 0;
	} else {
		return 1;
	}
}

// Print a string
void printString(char* str) {
	printf("%s", str);
	printf("\n");
}
