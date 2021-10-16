#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
int doesDirectoryExists(char*);
void printString(char*);
char** commandDelimeter(char*);
int countArgs(char*);
int countLongestArg(char*);

// Shell command prototypes
int movetodir(char*);
int whereami();
int start(char*);
int background(char*);
int dalek(int);
void byebye();

char* currentdir;

int main() {
	// Buffer to store recent commands
	Buffer * commandBuffer = createBuffer();
	// Path to the current working directory
	currentdir = getcwd(currentdir, 100);
	// Start the shell interface
	insideShell(commandBuffer);


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
			printf("Invalid command. Please try again.\n");	
		}
		isCommandValid = 0;
	}
}

int executeCommand(char* userInput) {
	// Max possible command length is 10 characters
	char command[11];
	int index = 0;
	int i;

	// Extract commmand from user input
	// Command is the first part of the users input up to the first space
	while(*userInput != ' ' && *userInput != '\n') {
		command[index++] = *userInput++;
	}
	command[index] = '\0'; 

	//If userInput has args, else userInput has 0 args
	if(*userInput != '\n'){
		*userInput++;
		index = 0;
		// Get the number of remaining elements in userInput 
		while(*userInput++ != '\n') {
			index++;
		}
		userInput -= (index+1);
		char args[index+1];
		// Extract '\n' from the end of userInput
		for(i = 0; i < index; i++) {
			args[i] = *userInput++;
		}
		args[i] = '\0';
		// Execute function
		if(strcmp(command, "movetodir") == 0) {
			return movetodir(args);
		}else if(strcmp(command, "start") == 0) {
			return start(args);
		}
	} else {
		char** argsArray = commandDelimeter(userInput);
		// Execute function
		if(strcmp(command, "whereami") == 0) {
			return whereami();
		} else if(strcmp(command, "byebye") == 0) {
			byebye();
		}
	}
	return 0;
}

// Returns array of user's input text
char* getUserCommand() {
	int stringSize = 64;
	int substringSize = 64;
	int substringIndex = 0;
	char substring[substringSize+1];
	char* stringRet = (char*)malloc(stringSize*sizeof(char));
	char c;
	// Dynamically storing user's command
	do {
		// Case when substring memory is full
		// Append substring to string and clear substring's memory
		if(substringIndex == (substringSize - 1)) {
			strcat(stringRet, substring);
			substringIndex = 0;     
			memset(substring, '\0', substringSize);
			// Case when string memory is full
			// Reallocate memory by a factor of 2
			if(substringIndex == (stringSize-1)) {
					stringSize *= 2;
					stringRet = realloc(stringRet, sizeof(char)*stringSize);
			}
		} 
		c = getchar();
		substring[substringIndex++] = c;
        } while(c != '\n');
	substring[substringIndex] = '\0';
	strcat(stringRet, substring);
	return stringRet;	
}

// If directory exists, currentdir gets assigned the string that gets passed in
int movetodir(char* directory) {
	int directoryExists = doesDirectoryExists(directory);
	if(directoryExists) {
		strcpy(currentdir, directory);
		return 1;
	} else {
		printf("Error, directory does not exists. Please try again\n");
		return 0;
	}
}

// Prints to the terminal current directory
int whereami() {
	if(currentdir != NULL) {
		printString(currentdir);
		return 1;
	} else {
		return -1;
	} 
}

// Starts a program with or without parameters
// Returns 0 when child process starts, Returns 1 when child is finshed and parent is finished waiting
// TODO: Test function with multiple args
int start(char* command){
	// pid uses status's value behind the scenes
	int status;
	int maxArgLength = 0;
	int argLength = 0;
	int index;

	// Split the user's command into seperate strings wherever the command has spaces
	char** usersArgs = commandDelimeter(command);
	int numberArgs = countArgs(command);
	// Calculate maxArgLength to allocate correct amount of memory
	for(index = 0; index < numberArgs; index++) {
		argLength = countLongestArg(usersArgs[index]);
		if(argLength > maxArgLength) {
			maxArgLength = argLength;
		}
	}
	// Allocate memory for the array that gets passed to exec() function
	char* writableUsersArgs[numberArgs+1];
	for(index = 0; index < numberArgs + 1; index++) {
		writableUsersArgs[index] = (char*)malloc(maxArgLength*sizeof(char));
	}
	// Copy the strings from the read only array to a writable array of strings
	for(index = 0; index < numberArgs; index++) {
		strcpy(writableUsersArgs[index], usersArgs[index]);
	}
	// Last entry of array must be NULL for exec() function to work properly
	writableUsersArgs[index] = NULL;
	
	// Create a child process
	pid_t pid = fork();
	if(pid == -1) {
		printf("Error forking");
		return -1;
	} else if(pid == 0) {
		// Jumps into new child process
		execv(writableUsersArgs[0], writableUsersArgs);
		return 0;
	} else {
		// Waits for child process to terminate before proceeding
		wait(&status);
		return 1;
	}
}

// TODO: Implement function
int background(char* command) {

}

// TODO: Implement function
int dalek(int pid){

}

// Exits the shell and saves the command history to a file
// TODO: write the command history to a file
void byebye() {
	exit(0);
}

// Splits a command into an array of all of its args
char** commandDelimeter(char* inputCommand) {
	char argument [50];
	int argsIndex = 0;
	int i;
	int commandLength = strlen(inputCommand);
	
	// Writable array that the command will be written to 
	char writableCommand[commandLength+1];
	for(i = 0; i < commandLength; i++) {
		writableCommand[i] = *inputCommand++;
	}
	writableCommand[i] = '\0';
	// Postion pointer back to first element of inputCommand array
	inputCommand -= commandLength;

	// Calculate number of args inputCommand has
	int numberArgs = countArgs(inputCommand);
	// Calculate the max length of all the args 
	int maxArgLength = countLongestArg(inputCommand);
	// Function allocates the array's memory based on the number of args and the max arg length
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
		argument[i] = '\0';
		// Position token pointer back to its first char element 
		token -= i;
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
	int argsCounter = 1;
	if(command == NULL) {
		return 0;
	} else {	
		while(*command != '\0') {
			if(*command++ == ' '){
				argsCounter++;
			}
		}
	}
	return argsCounter;
}

// Returns the length of the longest arg
int countLongestArg(char* command) {
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
int doesDirectoryExists(char* directory) {
	if(access(directory, F_OK) == 0){
		return 1;
	} else {
		return 0;
	}
}

// Prints a string
void printString(char* str) {
	printf("%s\n", str);
}
