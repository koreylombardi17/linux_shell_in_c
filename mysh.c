#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

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

int main() {
	// Instantiate and create the buffer to store recent commands
	Buffer * commandBuffer;
	commandBuffer = createBuffer();
	
	// Struct that holds metadata about directories
	struct stat s;
	
	// Gets the path to the current working directory
	char * currentdir = getcwd(currentdir, 100);


	// parent pid and child pid get printed
	// then child starts from the fork and becomes
	// a parent to a new child?
	pid_t pid = fork();
	printf("Parent's pid : %d\n", getppid());
	printf("Child's pid : %d\n", getpid());

	// Working on start application command	
	// This line of code launches vim from filesystem on eustis.
	// system("/usr/bin/vim");
	
	return 0;
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

