/*
#include <stdio.h>
#include <string.h>
#include "readFile.h"
*/
#include "main.h"

int processInputFile(FILE *plik) 
{

	char singleChar;
	int numberOfLines = 0;
	while((singleChar = fgetc(plik)) != EOF) {
		//printf("%c", singleChar);
		if (singleChar == '\n')
			numberOfLines++;
		}
	//printf("Number of lines in file = %d\n", numberOfLines);
	rewind(plik);
	return numberOfLines;
}
