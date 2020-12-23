#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#define MAXCOMMAND 100 // max number of letters to be supported
#define MAXWORD 20     // max number of words in the command to be supported
#define MAX_PATH 1024

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

// Global Variables
extern char **environ;

// set the shell env shell=<pathname>/myshell
void setShellEnv();

// Greeting shell during startup
void init_shell();

// Function to print Current Directory.
void printDir();

void remove_endOfLine(char* buf);

// Function to take input
int takeInput(char* str,int* batchFlag);

int environ_list(void);

// function for finding pipe
int parsePipe(char* str, char** strpiped);

//if '<' char was found in string inputted by user
void changeThe_inputFile(char* input_fileName);

//if '>' char was found in string inputted by user
void changeThe_outputFileTrunc(char* output_fileName);

//if '>>' char was found in string inputted by user
void changeThe_outputFileAppend(char* output_fileName);

// finds where '<' or '>' occurs and remove them with the inputFileName|outputFileName
// from the ards passed to execvp method, to ensure that it wont't read them
int checkFor_IOredirection(char** parsed);

// function for parsing command words
int parseSpace(char* str, char** parsed, int* parsedArgsLen );

// set the env  parent=<pathname>/myshell
void setParentEnv();

// check if there is ampersand at the end of the command line
int is_Background(char* lastStrInput) ;

// Function where the system command is executed
void execArgs(char** parsed,int* parsedArgsLen);

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe,int* parsedArgsLen);

int checkPathTochange (char *parsed[],char* dirpath );

void command_cd(char *parsed[]);

void Command_dir(char *parsed[],int* parsedArgsLen);

// Help command builtin
void openHelp(char **args);

// Function to execute builtin commands
int ownCmdHandler(char** parsed,int* parsedArgsLen);

int checkIfThereIsABatchFile(char** parsed);

int processString(char* str, char** parsed, char** parsedpipe,int* IORedirectionFlag,int* parsedArgsLen);
