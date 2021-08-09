#ifndef TABLESOPERATIONS_H
#define TABLESOPERATIONS_H

void createTable(FILE *plik, int *intFileData, int numberOfLines, char *charFileData);

void printError(int lineCounter);

int charToInt(char c);

void printCharTable(int numberOfLines, char *charFileData);

void printIntTable(int numberOfLines, int *intFileData);

void printComposedData(int numberOfLines, int *intFileData, char *charFileData);

void sortTables(int numberOfLines, int *intFileData, char *charFileData);

void swapInts(int *int1, int *int2);

void swapChars(char *char1, char *char2);

void checkData(int numberOfLines, int *intFileData);

void calculateTimeAndStatus(int *taskStatusesAndTimeBetweenThem, int numberOfLines, int *intFileData, int *tasksLeftForCompletion);

#endif
