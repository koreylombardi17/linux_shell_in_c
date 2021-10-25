#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <signal.h>
#include <sys/wait.h> 

typedef struct Buffer Buffer;

// ArrayList architecture
typedef struct Buffer {
    int size, initialSize, cap;
	char** arr;
} Buffer;

// Data structure prototypes
Buffer * createCommandBuffer();
Buffer* clearCommandBuffer(Buffer*);
void appendCommandToBuffer(Buffer*, char*);
void expandCommandBuffer(Buffer*);
char** createArgsArray(int, int);

// Helper function prototypes
void insideShell(FILE*, Buffer*);
char* getUserCommand();
int executeCommand(FILE*, Buffer*, char*);
int executeCommandWithArgs(char*, char*, Buffer*, FILE*, int);
int executeCommandNoArgs(char*, Buffer*, FILE*);
int doesDirectoryExists(char*);
void printString(char*);
char** commandDelimeter(char*);
int countArgs(char*);
int countLongestArg(char*);
int clearFile(FILE*, const char*, Buffer*);
int appendFile(FILE*, const char*, Buffer*);
void loadPreviousCommands(FILE*, Buffer*);

// Shell command prototypes
int movetodir(char*);
int whereami();
int start(char*);// Need to test multiple args
int background(char*, Buffer*, FILE*);// Need to implement
//int dalek(char*);// Need to implement
int printHistory(Buffer*);
int clearHistory(Buffer*, char*);
int replay(Buffer*, FILE*, char*);
void byebye();

char* currentdir;
//const char* commandFile = getcwd(currentdir, 100) + "recent_commands.txt";
const char* commandFile = "/home/pi/Desktop/c/hw2/linux_shell_in_c/recent_commands.txt";

int main() {
	// Buffer to store recent commands
	Buffer * commandBuffer = createCommandBuffer();	
	// Pointer used for file manipulation
	FILE* fptr;
	// Load previous commands from recent_commands file. Blank file will
	// be created if one doesnt exists
	loadPreviousCommands(fptr, commandBuffer);	
	
	// Path to the current working directory
	currentdir = getcwd(currentdir, 100);

	printf("%s\n", commandFile);


	// Start the shell interface
	insideShell(fptr, commandBuffer);


	// Dont forget to free memory	
	
	return 0;
}

// Shell's interface
void insideShell(FILE* fptr, Buffer* commandBuffer) {
	int isCommandValid;
	char* command;	
	
	while(1) {
		isCommandValid = 0;
		printf("#");
		command = getUserCommand();
		appendCommandToBuffer(commandBuffer, command);
		isCommandValid = executeCommand(fptr, commandBuffer, command);
		if(isCommandValid == 0) {
			printf("Invalid command. Please try again.\n");	
		} else if(isCommandValid == 2) {
			break;
		}
		isCommandValid = 0;
	}
}

// Function gets called inside the shell
// Returns 1 on successful execution, returns 0 on failure
int executeCommand(FILE* fptr, Buffer* commandBuffer, char* userInput) {
	// Max possible command length is 10 characters
	char command[11];
	int index = 0;
	int userInputIndex = 0;

	// Extract commmand from user input
	// Command is the first part of the users input up to the first space
	while(*userInput != ' ' && *userInput != '\n') {
		command[index++] = *userInput++;
	}
	command[index] = '\0'; 
	// Index used to position pointer back to its first character in userInput
	userInputIndex = index+1;

	//If userInput has no args, else userInput has args
	if(*userInput == '\n') {
		return executeCommandNoArgs(command, commandBuffer, fptr);
	} else {
		return executeCommandWithArgs(userInput, command, commandBuffer, fptr, userInputIndex);
	}
	return 0;
}

// Function executes user's command that contains args 
// Returns 1 on success, returns 0 on failure
int executeCommandWithArgs(char* userInput, char* command, 
							Buffer* commandBuffer, FILE* fptr, int userInputIndex) {
	// Currently pointing to char value of space. Position userInput to next letter
	*userInput++;
	int index = 0;
	int i;
	
	// Get the number of remaining elements in userInput 
	while(*userInput++ != '\n') {
		index++;
	}
	// Position userInput pointer to it's first char of it's args
	userInput -= (index+1);
	char args[index+1];
	
	// Extract '\n' from the end of userInput
	for(i = 0; i < index; i++) {
		args[i] = *userInput++;
	}
	args[i] = '\0';
	// Total number of chars in original userInput
	userInputIndex += index;
	// Position userInput pointer to its first char of the original userInput
	userInput -= (userInputIndex);

	// Enter shell function that contains args
	if(strcmp(command, "movetodir") == 0) {
		return movetodir(args);
	}else if(strcmp(command, "start") == 0) {
		return start(args);
	}else if(strcmp(command, "background") == 0) {
		return background(args, commandBuffer, fptr);
	}else if(strcmp(command, "replay") == 0) {
		return replay(commandBuffer, fptr, args);
	}else if(strcmp(command, "history") == 0) {
		// Copy userInput since clearHistory might erase memory that needs accessed in certains cases
		char userInputCopy[userInputIndex+1];
		strcpy(userInputCopy, userInput);
		int historyClearedSuccessfully = clearHistory(commandBuffer, args);
		if(historyClearedSuccessfully){
			// Checking for errors when appending file
			if(!clearFile(fptr, commandFile, commandBuffer)){
				printf("Error clearing command file.\n");
			} else {
				appendCommandToBuffer(commandBuffer, userInputCopy);
				return historyClearedSuccessfully;
			}
		} else {
			return 0;
		}
	} 
	//else if(strcmp(command, "dalek") == 0) {
		//return dalek(args);
	//}
}

// Function executes user's command that has 0 args 
// Returns 1 on success, returns 0 on failure
int executeCommandNoArgs(char* command, Buffer* commandBuffer, FILE* fptr) {
	// Enter shell function with no args
	if(strcmp(command, "whereami") == 0) {
		return whereami();
	} else if(strcmp(command, "history") == 0) {
		return printHistory(commandBuffer);
	}else if(strcmp(command, "byebye") == 0) {
		// Checking for errors when appending file
		if(!appendFile(fptr, commandFile, commandBuffer)) {
			// Print error message and exit shell
			printf("Error writing recent commands to file.\n");
			byebye();
		}
		// Exit the shell
		byebye();
	}
	return 0;
}

// Returns dynamic array of user's input text
char* getUserCommand() {
	int stringSize = 64;
	int substringSize = 64;
	int stringIndex = 0;
	int substringIndex = 0;
	char substring[substringSize+1];
	char* stringRet = (char*)malloc(stringSize*sizeof(char));
	stringRet = strcpy(stringRet, "");
	char c;
	// Dynamically storing user's command
	do {
		// Case when substring memory is full
		// Append substring to string and clear substring's memory
		if(substringIndex == (substringSize - 1)) {
			strcat(stringRet, substring);     
			memset(substring, '\0', substringSize);
			// Case when string memory is full
			// Reallocate memory by a factor of 2
			if(stringIndex == (stringSize-1)) {
				stringSize *= 2;
				stringRet = (char*)realloc(stringRet, sizeof(char)*stringSize);
			}
			substringIndex = 0;
			stringIndex++;
		} 
		c = getchar();
		substring[substringIndex++] = c;
		stringIndex++;
        } while(c != '\n');
	substring[substringIndex] = '\0';
	strcat(stringRet, substring);
	return stringRet;	
}

// If directory exists, currentdir gets assigned the string that gets passed in
// Returns 1 if the directory exists, returns 0 if not
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

// Prints to the terminal the current working directory
int whereami() {
	if(currentdir != NULL) {
		printString(currentdir);
		printf("\n");
		return 1;
	} else {
		return -1;
	} 
}

// Starts a program with or without parameters
// Returns 2 when child process starts, Returns 1 when child is finshed and parent is finished waiting
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
		return 2;
	} else {
		// Waits for child process to terminate before proceeding
		wait(&status);
		return 1;
	}
}

// TODO: Implement function
// Similar to the start command, but it immediately prints the PID of the program it 
// started, and returns the prompt. 
// Returns 2 when child process starts, Returns 3 when child is finshed and parent is finished waiting
int background(char* command, Buffer* commandBuffer, FILE* fptr) {
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
		return 1;
	} else {
		// Waits for child process to terminate before proceeding
		printf("PID = %d\n", pid);
		insideShell(fptr, commandBuffer);
		wait(&status);
		return 2;
	}
}

// TODO: Implement function
// Immediately terminate the program with the specific PID
// Returns 1 on successfull kill, returns 0 on failure
//int dalek(char* pidStr){
//	int pid = atoi(pidStr);
//	if(kill(pid, SIGKILL) == 0) {
//		printf("Kill successful.\n");
//		return 1;
//	} else {
//		printf("Kill failed.\n");
//		return 0;
//	}
//}

// Re-executes the command labeled with its number in the history 
// Returns 1 on succesful execution, 0 on failure
int replay(Buffer* commandBuffer, FILE* fptr, char* args) {
	int commandIndex = atoi(args);
	// Index shown to user is in reverse order of arraylist in order to print most recent commands
	// Move to the actual index
	commandIndex = (commandBuffer->size - commandIndex) - 2;
	return executeCommand(fptr, commandBuffer, commandBuffer->arr[commandIndex]);
}

// Returns 1 on printing command history successfully, return 0 on failure
int printHistory(Buffer* commandBuffer) {
	if(commandBuffer == NULL) {
		return 0;
	} else {
		int index = 0;
		for(int i = (commandBuffer->size - 1); i >= 0; i--) {
			printf("%d: ", index++);
			printString(commandBuffer->arr[i]);
		}	
	}
	return 1;
}

// Returns 1 if history cleared successfully, returns 0 on error
int clearHistory(Buffer* commandBuffer, char* arg) {
	if(commandBuffer == NULL || strcmp("-c", arg) != 0) {
		return 0;
	} else {
		commandBuffer = clearCommandBuffer(commandBuffer);
	}
	return 1;
}

// Exits the shell
void byebye() {
	exit(0);
}

// Deletes any prior text contained in the file. If no file, it will create one.
// Returns 1 if success, returns 0 if failed
int clearFile(FILE* fptr, const char* commandFile, Buffer* commandBuffer) {
	fptr = fopen(commandFile, "w");
	if(fclose(fptr) == 0) {
		return 1;
	}
	return 0;
}

// Appends the existing file or creates a new file if one doesn't exists
// Returns 1 if success, returns 0 if failed
int appendFile(FILE* fptr, const char* commandFile, Buffer* commandBuffer) {
	fptr = fopen(commandFile, "a");
	for(int i = commandBuffer->initialSize; i < commandBuffer->size; i++) {
		fprintf(fptr, "%s", commandBuffer->arr[i]);
	}
	if(fclose(fptr) == 0) {
		return 1;
	}
	return 0;
}

// Loads the user's recent commands into the command buffer and set the command
// buffers initial size to the number of commands loaded into buffer
void loadPreviousCommands(FILE* fptr, Buffer* commandBuffer) {
	int stringSize = 64;
	int substringSize = 64;
	int substringIndex;
	int numCommandsLoaded = 0;
	char substring[substringSize+1];
	char* stringRet;
	fptr = fopen(commandFile, "r");
	if(fptr == NULL) {
		return;
	}
	// Has to be signed char or it can cause seg fault depending on the system 
	signed char c = fgetc(fptr);

	// Dynamically storing user's command
	while(c != EOF) {
		substringIndex = 0;
		stringRet = (char*)malloc(stringSize*sizeof(char));
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
			substring[substringIndex++] = c;
			c = fgetc(fptr);
			} while(c != '\n');
		numCommandsLoaded++;
		substring[substringIndex++] = c;
		substring[substringIndex] = '\0';
		// Concatenate stringRet with substring
		strcat(stringRet, substring);
		appendCommandToBuffer(commandBuffer, stringRet);
		c = fgetc(fptr);
		// Free memory for both arrays
		memset(substring, '\0', substringSize);
		free(stringRet);
	}
	commandBuffer->initialSize = numCommandsLoaded;
	fclose(fptr);
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
Buffer* createCommandBuffer() {
	Buffer * ret = calloc(1, sizeof(Buffer));
	ret->size = 0;
	ret->cap = 8;
	ret->arr = calloc(ret->cap, sizeof(char *));
	for(int i = 0; i < ret->cap; i++) {
		ret->arr[i] = calloc(100, sizeof(char));
	}
	return ret;
}

// Clears the command buffer
Buffer* clearCommandBuffer(Buffer* commandBuffer){
	for(int i = 0; i < commandBuffer->cap; i++) {
		free(commandBuffer->arr[i]);
	}
	commandBuffer->size = 0;
	commandBuffer->initialSize = 0;
	commandBuffer->cap = 8;
	commandBuffer->arr = calloc(commandBuffer->cap, sizeof(char *));
	for(int i = 0; i < commandBuffer->cap; i++) {
		commandBuffer->arr[i] = calloc(100, sizeof(char));
	}
	return commandBuffer;
}

// Append command to Buffer
void appendCommandToBuffer(Buffer* commandBuffer, char* command) {
   	// Location where command will be appended
	int index = commandBuffer->size;
	int stringLength = strlen(command);

	// If buffer reaches capacity, expand size of buffer
	if(index >= commandBuffer->cap) {
		expandCommandBuffer(commandBuffer);
	}

	// commandBuffer->arr[i] has 100 chars allocated. If stringLength is > 99, reallocate
	if(stringLength > 98) {
		commandBuffer->arr[index] = (char*)realloc(commandBuffer->arr[index], (stringLength+1)*sizeof(char));
	}
	// Append command, increment size by 1
	strcpy(commandBuffer->arr[index], command);
	commandBuffer->size++;
}

// Reallocate memory by a multiple of two
void expandCommandBuffer(Buffer* commandBuffer) {
    commandBuffer->cap = commandBuffer->cap * 2;
	commandBuffer->arr = realloc(commandBuffer->arr, commandBuffer->cap * sizeof(char *));
	for(int i = commandBuffer->size; i < commandBuffer->cap; i++) {
		commandBuffer->arr[i] = calloc(100, sizeof(char));
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

// Prints a string without newline since user's input contains new line
void printString(char* str) {
	printf("%s", str);
}
