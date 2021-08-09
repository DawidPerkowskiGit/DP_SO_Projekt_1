#include "main.h"


void createTable(FILE *plik, int *intFileData, int numberOfLines, char *charFileData) 
{
	char singleChar;
	int colonCounter = 0, lineCounter = 0, convertedToInt = 0;

	//DEBUG
	//printf("Przed wypelnieniem");
	//printCharTable(numberOfLines, (char *)charFileData);

	while((singleChar = fgetc(plik)) != EOF) 
	{
		printf("%c", singleChar);
		if (singleChar == ':') // interesujaca nas dane sa podzielone dwukropkami, wiedzac jakie dane znajduja sie po n-tym
		{	// z kolei dwukropku w danej linii mozemy zapisac je w odpowiednie mijesca w tablicach
			colonCounter++;
			convertedToInt = 0;
		}
		else if (singleChar == '\n' && colonCounter == 3) 
		{ // koniec linii, zerowanie zmiennych
			lineCounter++;
			colonCounter = 0;
			convertedToInt = 0;
		}
		else if (singleChar == '\n' && colonCounter != 3) // jezeli w pliku jest wiecej lub mniej dwukropkow od 3
		{						// i linia sie konczy to plik zawiera niepoprawnie zapisane dane
			printError(lineCounter);
		}
		else //wypelnienie poszczegolnych tabel odpowiednimi danymi
		{
			if (colonCounter == 0 || colonCounter == 1) // zapisujemy goidzny i minuty do tabeli
			{
				// dane z pliku odczytujemy znak po znaku od lewej do prawej
				// jesli chcemy zapisac liczbe dwucyfrowa na przyklad godzine to 
				// musimy to zrobic w dwoch krokach. Najpierw zapisujemy liczbe dziesiatek, a potem liczbe jednosci
				// wiec nasza zmienna bedzie miala wartosc element_0 * 10 + element_1
				
				if ((convertedToInt = charToInt(singleChar)) < 10 ) //wstepne sprawdzanie poprawnosci danych
				{ 						// czy znak jest cyfra
					*(intFileData + lineCounter * SIZE_OF_INT_DATA + colonCounter) = *(intFileData + lineCounter * SIZE_OF_INT_DATA + colonCounter) * 10 + convertedToInt;
				}
				else
				{
					printError(lineCounter);
				}
			
			}
			else if (colonCounter == 2) // zapisujemy nazwe programu do tablicy char
			{
				// uzywajac funkcji strcat kopiujemy znak po znaku do naszej tabeli przechowujacej nazwy programow
				strcat(charFileData+lineCounter*SIZE_OF_CHAR_DATA, &singleChar);
			}
			else if (colonCounter == 3)
			{ // zapisanie "info" do tablicy
				convertedToInt = charToInt(singleChar);
				if ((convertedToInt = charToInt(singleChar)) < 10 ) //wstepne sprawdzanie poprawnosci danych
				{ 						// czy znak jest cyfra
					*(intFileData + lineCounter * SIZE_OF_INT_DATA + colonCounter-1) = *(intFileData + lineCounter * SIZE_OF_INT_DATA + colonCounter-1) * 10 + convertedToInt;
				}
				else 
				{
					printError(lineCounter);
				}
			}
		}		
	}
	rewind(plik);
}

void printError(int lineCounter) { 
	printf("\nDane zawarte w pliku sa bledne, problem w linii #%d\n", lineCounter+1);
	syslog(LOG_NOTICE, "Dane zawarte w pliku sa bledne, problem w linii #%d\n", lineCounter+1);
	exit(EXIT_FAILURE);
}

int charToInt(char c) { // zamiana cyfry typu char na cyfre typu int
	int tempInt = 0;
	tempInt = c - '0';
	return tempInt;
}

void printCharTable(int numberOfLines, char *charFileData) // drukowanie danych zawartych w tabeli int, element po elemencie
{
	for (int i = 0 ; i < numberOfLines ; i++)
	{
		printf("charFileData[%d] = %s\n", i, charFileData+i*SIZE_OF_CHAR_DATA);
	}
}

void printIntTable(int numberOfLines, int *intFileData) // drukowanie danych zawartych w tabeli char element po elemencie
{
	for (int i = 0 ; i < numberOfLines * SIZE_OF_INT_DATA ; i++)
	{
		printf("intFileData[%d] = %d\n", i, *(intFileData+i));
	}
}

void printComposedData(int numberOfLines, int *intFileData, char *charFileData) // drukowanie danych zawartych w obu tabelach
{										// linia po linii
	for (int i = 0 ; i < numberOfLines ; i++)
	{
		printf("%d = %d : %d : ", i, *(intFileData + i * SIZE_OF_INT_DATA + 0),*(intFileData + i * SIZE_OF_INT_DATA + 1));
		printf("%s :", charFileData + i * SIZE_OF_CHAR_DATA);
		printf(" %d\n", *(intFileData + i * SIZE_OF_INT_DATA + 2));
	}
}


void sortTables(int numberOfLines, int *intFileData, char *charFileData) // chronologiczne sortowanie danych
{									// algorytm - sortiwanie przez wybieranie
	int tempInt, index, minimum;					// wartoscia wybierana jest wartosc najmniejsza
	//char tempChar[SIZE_OF_CHAR_DATA];
		
	for (int j = 0 ; j < numberOfLines ; j++)
	{
		minimum = 24; // sorujemy godzinami, wiec jesli dane sa poprawne to nie znjadziemy w nich godziny 24 lub wyzszej
		for (int i = j ; i < numberOfLines ; i++) // wiec kazda poprawnie zapisana godzina bedzie mniejsza od 24
		{	
			if (*(intFileData + i * SIZE_OF_INT_DATA ) < minimum ) // jezeli obecny element jest mniejszy od
			{						// minimum to zapamietaj jego pozycje
				minimum = *(intFileData + i * SIZE_OF_INT_DATA );
				index = i;	
			}
			else if (*(intFileData + i * SIZE_OF_INT_DATA ) == minimum ) // jesli element jest rowny
			{						// obecnemu minimum, czyli jesli godziny sa takie same
									// to porownaj minuty i zapamietaj mniejszy element
				if (*(intFileData + i * SIZE_OF_INT_DATA + 1 ) < *(intFileData + index * SIZE_OF_INT_DATA + 1))
				{
					index = i;
				}
			}
		}
		// wstawienie najmnijeszego elemntu z danej iteracji na j-ta pozycje
		swapInts(intFileData + j * SIZE_OF_INT_DATA, intFileData + index * SIZE_OF_INT_DATA);
		swapInts(intFileData + j * SIZE_OF_INT_DATA + 1, intFileData + index * SIZE_OF_INT_DATA + 1);
		swapInts(intFileData + j * SIZE_OF_INT_DATA + 2, intFileData + index * SIZE_OF_INT_DATA + 2);
		swapChars(charFileData + SIZE_OF_CHAR_DATA * j, charFileData + SIZE_OF_CHAR_DATA * index);

	}
}

void swapInts(int *int1, int *int2) // zamiana miejscami dwoch zmiennych typu int
{
	int tempInt;

	tempInt = *int1;
	*int1 = *int2;
	*int2 = tempInt;
}

void swapChars(char *char1, char *char2) // zamiana miejscami dwoch zmiennych typu char
{
	char tempChar[SIZE_OF_CHAR_DATA];
	strcpy(tempChar, "");
	
	strcpy(tempChar, char1);
	strcpy(char1, char2);
	strcpy(char2, tempChar);
}

void checkData(int numberOfLines, int *intFileData) // sprawdzanie czy godziny sa z przedziału <00;23> , minuty <00;59> , 
{											// info {0 , 1 , 2 }
	for (int i = 0 ; i < numberOfLines; i++)
	{
		if ( *(intFileData + i * SIZE_OF_INT_DATA) > 23 || *(intFileData + i * SIZE_OF_INT_DATA) < 0 ||*(intFileData + 1 + i * SIZE_OF_INT_DATA) > 59 || *(intFileData + 1 + i * SIZE_OF_INT_DATA) < 0 ||*(intFileData + 2 + i * SIZE_OF_INT_DATA) < 0 || *(intFileData + 2 + i * SIZE_OF_INT_DATA) > 2) 
		{
			printError(i);
		}
	}
}

void calculateTimeAndStatus(int *timeBetweenTasks, int numberOfLines, int *intFileData, int *tasksLeftForCompletion) 
{
	int timeDiff;

	time_t now = time(0); // czas w sekundach ktory uplynal od 01.01.1970 00:00:00 UTC

	//printf("Time??? = %d\n", now);

	now = now - now % 60; // ucinamy sekundy z czasu 
	now = now / 60; // czas w minutach pomiedzy 01.01.1970 00:00 a obecna data
	now = (now  + 120) % 1440 ; //nie interesuje nas data tylko godziny i minuty,
			// a 24 * 60 = 1440 wiec zostawiamy tylko wartosc w minutach, dodajemy 120 bo GMT+2
	//printf("\nTime now = %d : %d\n", now/60, now%60);

	for (int i = 0 ; i < numberOfLines ; i++)
	{
	 	// dla pierwszego elementu przeliczamy czas z godzin i minut na minuty 
		// i odejmujemy od niego obecny czas w minutach
		if (i == 0 || *(timeBetweenTasks + (i - 1)) < 1 )
		{
			timeDiff = *(intFileData + i * SIZE_OF_INT_DATA) * 60 + *(intFileData + i * SIZE_OF_INT_DATA + 1) - now;
		}
		else
		{
			timeDiff = (*(intFileData + i * SIZE_OF_INT_DATA) * 60 + *(intFileData + i * SIZE_OF_INT_DATA + 1)) - (*(intFileData + (i - 1) * SIZE_OF_INT_DATA) * 60 + *(intFileData + (i - 1) * SIZE_OF_INT_DATA + 1));
		}
					
		*(timeBetweenTasks + i) = timeDiff;

		if (timeDiff > 0 && *tasksLeftForCompletion < 1)
		{
			*tasksLeftForCompletion = numberOfLines - i;
		}
		
	
	printf("Zadanie #%d, Czas do wykonania w minutach = %d\n",i+1, *(timeBetweenTasks + i));
	}
}
