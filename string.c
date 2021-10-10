#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printString(char[]);
char * getString(char []);
char ** getArray(char* []);
void printAddress(char **);
char** allocateStringArray(int);

int main() {

	
	char** stringArray = allocateStringArray(10); 
	strcpy(stringArray[0], "Korey Lombardi is up late tonight coding.");
	printf("%s\n", stringArray[0]);
	
	return 0;
}

void printAddress(char ** usersArgs) {
	usersArgs = (char**)malloc(5 * sizeof(char*));
	printf("%p", usersArgs);
}

void printString(char string[]){
	printf("%s\n", string);
}

char * getString(char string[]){
	return string;
}

char ** getArray(char* array[]){
     return array;
}

char** allocateStringArray(int numberStrings){
	char ** stringArray = (char**)malloc(sizeof(char*));
	for(int i = 0; i < numberStrings; i++) {
		stringArray[i] = (char*)malloc(50*sizeof(char));
	}
	return stringArray;
}
