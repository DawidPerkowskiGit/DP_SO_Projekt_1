# DP_SO_Projekt_1
Repozytorium przechowujące projekt z Systemów Operacyjnych

Tematem tego projektu był demon "Minicron". Jego zadaniem jest uruchamianie programów o określonych godzinach.

Program napisany w języku C, mozna go uruchomić na systemach operacyjnych opartych o Uniksa.

Temat zadania : 
Demon uruchamiany w sposób następujący:

./minicron <taskfile>
Plik taskfile zawiera zadania zapisane w następującym formacie:

<hour>:<minutes>:<command>:<info>
Przy czym command jest dowolnym programem.

Parametr info może mieć jedną z trzech wartości:
0 - użytkownik chce otrzymać treść, jaką polecenie wypisało na standardowe wyjście (stdout)
1 - użytkownik chce otrzymać treść, jaką polecenie wypisało na wyjście błędów (stderr).
2 - użytkownik chce otrzymać treść, jaką polecenie wypisało na standardowe wyjście i wyjście błędów.
Demon wczytuje zadania z pliku i porządkuje je chronologicznie, a następnie zasypia. Budzi się, kiedy nadejdzie czas wykonania pierwszego zadania, tworzy proces potomny wykonujący zadanie, a sam znowu zasypia do czasu kolejnego zadania. Proces potomny wykonuje zadanie uruchamiając nowy proces.

Demon kończy pracę po otrzymaniu sygnału SIGINT i ewentualnym zakończeniu bieżącego zadania (zadań). Informacje o uruchomieniu zadania i kodzie wyjścia zakończonego zadania umieszczane są w logu systemowym.
