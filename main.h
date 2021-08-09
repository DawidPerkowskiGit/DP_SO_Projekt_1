#ifndef MAIN_H
#define MAIN_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include "readFile.h"
#include "tablesOperations.h"
#include <time.h>
#include <syslog.h>
#include <signal.h>

#define MAX_DL 				256	// maksymalna dlugosc nazwy pliku
#define SIZE_OF_INT_DATA 		3	// tablica int bedzie przechowywala 3 elementy w jednej linii - godzina, minuty, info
#define SIZE_OF_CHAR_DATA 		256	// tablica komend bedzie przechowywala nazwy programow o maksymalnej dlugosci 32 znakow
#define EXECUTION_TIME_MULTIPLIER 	1	// zmienna podawana do rozkazu "sleep" jesli chcemy aby program uruchamial sie o zadanej, reczywistej godzinie to nalezy ustawic ta wartosc na 60, w celu sprawdzenia dzialania programu ustawiono zmienna na 1
#endif
