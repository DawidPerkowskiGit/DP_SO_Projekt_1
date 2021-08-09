
#include "main.h"

				

void executeCommand(int argNumber, char charFileData[SIZE_OF_CHAR_DATA], int info);

void handler(int signum)
{
	if (signum == SIGINT)
	{
		syslog(LOG_NOTICE, "Zakonczono program poprzez sygnal SIGINT\n");
		exit(signum);
	}
}


int main(int argc, char *argv[]) {

	setlogmask(LOG_UPTO (LOG_NOTICE));
	openlog ("Logowanie z minicron", LOG_CONS | LOG_PID, LOG_LOCAL1);

	syslog(LOG_NOTICE, "Program started by User %d\n", getuid());
	syslog(LOG_INFO, "Poczatek logowania");

	signal(SIGINT, handler);
	
    	
	char nazwa_pliku[MAX_DL];
	FILE *plik;
	plik=fopen(argv[1], "r");

	printf(" =========================== \n          %s          \n ===========================\n", argv[1]);	

	if (plik == NULL) //otwarcie pliku z lista zadan do wykonania
	{ // jesli plik nie isntnieje to wypisz info
		perror("Blad podczas otwierania pliku");
		syslog(LOG_NOTICE, "Plik nie istnieje, zakonczono dzialanie programu - exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
		exit(EXIT_FAILURE);
	}
	else 
	{// jesli istnieje to wypisz jego zawartosc
		int numberOfLines = 0;
		numberOfLines = processInputFile(plik);
		
		if (numberOfLines < 1) { //sprawdzamy czy plik jest pusty
			perror("Plik jest pusty, koniec dzialania programu");
			syslog(LOG_NOTICE, "Plik jest pusty, zakonczono dzialanie programu - exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
			exit(EXIT_FAILURE);
		}
		else 	 
		{	// jesli wszystko jest w porzadku, to tworzymy tabele na dane
			int intFileData[numberOfLines][SIZE_OF_INT_DATA]; //<godziny>, <minuty>, <info>
			char charFileData[numberOfLines][SIZE_OF_CHAR_DATA]; // <command>

			// zerowanie elementow tabeli przchowujacej int
			for (int i = 0; i < numberOfLines ; i++ )
			{
				for (int j = 0; j < SIZE_OF_INT_DATA ; j++ )
				{
					intFileData[i][j] = 0;
				}
			}

			// zerowanie elementow tabeli przchowujacej char
			for (int i = 0 ; i < numberOfLines ; i++)
			{
				strcpy(charFileData[i], "");
			}
				
			// wypelenienie tablic danymi pobranymi z pliku			
			createTable(plik, (int *)intFileData, numberOfLines, (char *)charFileData);

			fclose(plik);

			//sprawdzenie poprawnosci danych
			checkData(numberOfLines, (int *)intFileData);

			
			// sortowanie zadan
			sortTables(numberOfLines, (int *)intFileData, (char *)charFileData);
			
			//DEBUG
			printf("\nPo sortowaniu\n");
			printComposedData(numberOfLines, (int *)intFileData, (char *)charFileData);

			

			int timeDiff, tasksLeftForCompletion = 0; // timeDiff roznica pomiedzy czasem obecnym i zadania

			int timeBetweenTasks[numberOfLines]; // wiersz 0 - status, wiersz 1 - czas

			calculateTimeAndStatus((int *)timeBetweenTasks, numberOfLines, (int *)intFileData, &tasksLeftForCompletion); // funkcja obliczajaca czas dzielacy poszczegolne wykonania i okreslajaca ich status

	
			/* Our process ID and Session ID */
			pid_t pid, sid;
				
			/* Fork off the parent process */
			pid = fork();
				
			if (pid < 0) 
			{
				syslog(LOG_NOTICE, "Powstał problem przy tworzeniu procesu potomka - demona, zakocznono dzialanie programu - exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
				perror("PID error : ");
				exit(EXIT_FAILURE);
			}
			else if (pid > 0)  // proces rodzic , aby nie blokowac terminala proces rodzic zakonczy swoje dzialanie
			{
				
			}
			else if (pid == 0) // proces potomny - demon
			{
				/* Create a new SID for the child process */
				sid = setsid();
				if (sid < 0) 
				{
					perror("SID error : ");
					syslog(LOG_NOTICE, "Powatal problem z przydzieleniem SID prcoesowi potomka, zakonczono dzialanie programu - exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
					exit(EXIT_FAILURE);
				
				}
						
				/* Change the current working directory */
				if ((chdir("/")) < 0) 
				{
					perror("chdir error");
					syslog(LOG_NOTICE, "Problem z zmiana biezacej sciezki dostepu, zamknieto program -  exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
					exit(EXIT_FAILURE);
				}
				while (tasksLeftForCompletion > 0)
				{
					int currentLine; // obecny indeks wykonywanego zadania
					currentLine = numberOfLines - tasksLeftForCompletion;
					syslog(LOG_NOTICE, "Zadanie %s czeka na wykonaie \n",charFileData[currentLine]);
					sleep(timeBetweenTasks[currentLine] * EXECUTION_TIME_MULTIPLIER);
						
					
					
					if (intFileData[currentLine][2] == 0) 
					{
						close(STDERR_FILENO);
					}
					else if (intFileData[currentLine][2] == 1)
					{
						close(STDOUT_FILENO);
					}
					
					pid_t pid_ch, sid_ch;

					pid_ch = fork();

					if (pid_ch < 0) 
					{
						syslog(LOG_NOTICE, "Powstał problem przy tworzeniu procesu potomka - demona, zakocznono dzialanie programu - exit(EXIT_FAILURE) = %d\n",EXIT_FAILURE);
						perror("PID error : ");
						exit(EXIT_FAILURE);
					}
					else if (pid_ch > 0)  // proces potomek - demon dzialajacy w tle
					{
						tasksLeftForCompletion--;
					}
					else if (pid_ch == 0) // proces potomny, ktory obsluguje wykonywanie sie polecen
					{
						printf("######  Zadanie : %s\n", charFileData[currentLine]);
						syslog(LOG_NOTICE, "Zadanie %s zostalo zakonczone powodzeniem\n",charFileData[currentLine]);
						executeCommand(currentLine, charFileData[currentLine], intFileData[currentLine][2]);

					}
				}
			}
			
		}
	}
	sleep(0.10);
	syslog(LOG_NOTICE, "Zakonczono wykonanie wszystkich zadan\n");
	exit(EXIT_SUCCESS);
}

void executeCommand(int currentLine, char charFileData[SIZE_OF_CHAR_DATA], int info) 
{
	
	char splitCommand[11][SIZE_OF_CHAR_DATA]; // tablica w ktorej bedzie znajdowac sie rozkaz oraz jego argumenty(maks 10)

	for (int i = 0 ; i < 11 ; i++) 
	{
		strcpy(splitCommand[i],"");
	}

	
	int argumentIndex = 0; // kazda spacja oddziela argument, wiec licznik spacji mozna uzyc przy kopiowaniu argumentu do 
				// odpowiedniego elementu tablicy
	int startOfString = 0; // od ktorego elementu zaczac kopiowac

	int beginOverwriteFrom = 0; // od ktorego elementu danego wiersza argumentow usuwac zbedne znaki

	for (int i = 0 ; i < strlen(charFileData)+1 ; i++) // przejdz caly wiersz
	{		
		
		if (charFileData[i] == ' ' || i == strlen(charFileData) ) // jezeli trafimy na spacje
		{
			strcat(splitCommand[argumentIndex],&charFileData[startOfString]); // to kopiujemy znaki od miejsca
			// o indeksie startOfString do konca
			for (int j = beginOverwriteFrom ; j < SIZE_OF_CHAR_DATA ; j++) // zerujemy znaki po spacji
			{							// co zostawi nam pojedyncze wyrazenie
				splitCommand[argumentIndex][j] = '\0';
			}
			argumentIndex++;
			startOfString = i+1;

			beginOverwriteFrom = 0;
		}
		else
		{
			beginOverwriteFrom++;
		}
	}
	int err;
	
	switch(argumentIndex - 1) {
		case 0:
		err = execlp(splitCommand[0], splitCommand[0], 0);
		break;
		
		case 1:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], 0);
		break;

		case 2:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], 0);
		break;

		case 3:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], NULL);
		break;

		case 4:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], NULL);
		break;

		case 5:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], NULL);
		break;

		case 6:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], splitCommand[6], NULL);
		break;

		case 7:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], splitCommand[6], splitCommand[7], NULL);
		break;

		case 8:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], splitCommand[6], splitCommand[7], splitCommand[8], NULL);
		break;

		case 9:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], splitCommand[6], splitCommand[7], splitCommand[8], splitCommand[9], NULL);
		break;

		case 10:
		err = execlp(splitCommand[0], splitCommand[0], splitCommand[1], splitCommand[2], splitCommand[3], splitCommand[4], splitCommand[5], splitCommand[6], splitCommand[7], splitCommand[8], splitCommand[9], splitCommand[10], NULL);
		break;
	}
	syslog(LOG_NOTICE, "Komenda execlp nie wykonala sie, kod bledu = %d\n", err);
	perror("execlp Error : ");

}

