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
void insideShell(Buffer*);
char* getUserCommand();
int executeCommand(char *);
void printCommands(Buffer*);
int doesDirectoryExist(char*, struct stat);
void printString(char*);
char** commandDelimeter(char*);
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
	
	// Start the shell interface
	//insideShell(commandBuffer);

	//start("/usr/bin/vim");
	// Dont forget to free memory	
	return 0;
}

void insideShell(Buffer* commandBuffer) {
	int isCommandValid;
	char* command;

	while(1) {
		isCommandValid = 0;
		printf("#");
		command = getUserCommand();
		appendCommandToBuffer(commandBuffer, command);
		isCommandValid = executeCommand(command);
		if(!isCommandValid) {
			printf("Invalid command. Please try again.");	
		}	
	}
}

//Todo: Split command into 2 elements array. The first part is the users command that will be used in strcmp. 
//	The second part gets sent to command delimeter, which then goes to the actual funtion itself. 
// 	Fix string delimeter from clipping first letter. Run in debug mode to fix this.
int executeCommand(char* userInput) {
	// Max possible command length is 10 characters
	char command[11];
	int i = 0;

	// Extract commmand from user input
	while(*userInput != ' ') {
		command[i++] = *userInput++;
	}
	command[i] = '\0';
	*userInput++; 

	char** argsArray = commandDelimeter(userInput);
	if(strcmp(command, "start") == 0) {
		start(command);
		return 1;
	} else {
		return 0;
	}
	return 0;
}

char* getUserCommand() {
	int stringSize = 64;
    int substringSize = 64;
    int substringIndex = 0;
    char substring[substringSize+1];
	char* string = (char*)malloc(stringSize*sizeof(char));
	char c;
	do {
		if(substringIndex == (substringSize - 1)) {
			strcat(string, substring);
			substringIndex = 0;     
			memset(substring, '\0', substringSize);
			if(substringIndex == (stringSize-1)) {
					stringSize *= 2;
					string = realloc(string, sizeof(char)*stringSize);
			}
		} 
		c = getchar();
		substring[substringIndex++] = c;
        } while(c != '\n');
	substring[substringIndex] = '\0';
    strcat(string, substring);
    return string;	
}

// Prints to the terminal current directory
// TODO: Make function return int

void whereami(char* currentdir) {
	printString(currentdir);
}

// Starts a program with or without parameters
// TODO: Make function return int
// TODO: Test function with multiple args
void start(char* command){
	// pid uses this value behind the scenes
	int status;
	int i;

	char** usersArgs = commandDelimeter(command);
	int numberArgs = countArgs(command);
	char* writableUsersArgs[numberArgs];
	for(i = 0; i < numberArgs; i++) {
		strcpy(writableUsersArgs[i], usersArgs[i]);
	}
	writableUsersArgs[i] = NULL;

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
char** commandDelimeter(char* command) {
	char argument [50];
	int argsIndex = 0;
	int counter = 0;
	int i;

	while(*command != '\0') {
		counter++;
		*command++;
	}
	command -= counter;
	
	char writableCommand[counter];
	for(i = 0; i < counter; i++) {
		writableCommand[i] = *command++;
	}
	command -= counter;

	int numberArgs = countArgs(command);
	int maxArgLength = countLongestArg(command);
	char** usersArgs = createArgsArray(numberArgs, maxArgLength);

	// Pointer used to iterate through through command stopping at all spaces
	char* token = strtok(writableCommand, " ");
	
	i = 0;
	// Split string wherever there are spaces
	while(token != NULL) {
		// Extracting string
		while(*token != '\n' && *token != '\0'){
			argument[i++] = *token++;
		}
		token -= i;
		argument[i] = '\0';
		strcpy(usersArgs[argsIndex++], argument);
		// Pointing to next string
		token = strtok(NULL, " ");
		i = 0;
		// Clearing out argument string
		while(argument[i] != '\0') {
			argument[i++] = '\0';
		}
		i = 0;
	}

	// exec() requires NULL as last value of array
	usersArgs[argsIndex] = NULL;
	return usersArgs;
}

// Returns the number of args contained in command
int countArgs(char* command) {
	int argsCounter = 0;
		
	while(*command != '\0') {
		if(*command++ == ' '){
			argsCounter++;
		}
	}
	// +1 for NULL value at end of the array
	return (argsCounter+1);
}

// Returns the length of the longest arg
int countLongestArg(char* command) {
	char delimeter[] = " ";
	int maxArgLength = 0;
	int argLength = 0;
	
	while(*command != '\0'){
		if(*command++ == ' ') {
			if(argLength > maxArgLength) {
				maxArgLength = argLength;
			}
			argLength = 0;
		} else {
			argLength++;
		}
	}
	// +1 for NULL character value at end of string
	return (maxArgLength+1);
}

// Function allocates memory for array of args
char** createArgsArray(int numberArgs, int maxArgLength) {
	char** argsArray = (char**)malloc(numberArgs * sizeof(char*));
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
