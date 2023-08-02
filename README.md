# Content of project
* [About application](#about-application)
* [Technologies](#technologies)
* [Setup](#setup)
* [More detailed information about application](#more-detailed-information-about-application)
* [Application view](#application-view)

## About application
This is project of a deamon which runs bash shell commands at chosen time. Tasks and their launch time are stored in a text file. 

## Technologies
C, Linux, Bash

## Setup
Using any command line interface perform tasks listed below.

1. Clone the repository  
```
git clone https://github.com/DawidPerkowskiGit/DP_SO_Projekt_1
```
2. Create and populate tasks file
```
touch tasks.txt
```
3. Build binary files
```
make minicron
```
4. Launch the demon
```
./minicron tasks.txt
```

## More detailed information about application

Tasks file format is as follows:   
hour:minutes:command:info   
hour - at which hour launch the task   
minutes - at which minute of an hour launch task   
command - any bash shell command   
info - parameter which describes how user wants to recieve additional task information.   
0 - user wants to recieve stdout information   
1 - user wants to recieve stderr information   
2 - user wants to recieve both stdout and stderr information   

Deamon reads file and chronogically sorts the task list. It goes to sleep and wakes up at the time when the first task has to be performed. It creates child process which executes the task and goes to sleep again. Deamon finishes work when performing all the tasks or recieving SIGINT signal. Additional ifrmation is printed in the terminal.

## Application view

![obraz](https://github.com/DawidPerkowskiGit/DP_SO_Projekt_1/assets/87314459/007782a6-ad0f-490c-bad5-1ea217508123)

![obraz](https://github.com/DawidPerkowskiGit/DP_SO_Projekt_1/assets/87314459/95faf059-91b3-433e-a9dd-6b60cc320f1c)

