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
void setShellEnv(){
  char shell_path[MAX_PATH]="shell=";
  char pwd[MAX_PATH];
  strcpy(pwd,getenv("PWD"));
  strcat(shell_path,pwd);
	strcat(shell_path,"/myshell");
  putenv(shell_path);
}

// Greeting shell during startup
void init_shell(){
	clear();
	printf("\n\n\n\t+++++++++++++++OUR SHELL++++++++++++++++++++");
	printf("\n\n\n\t+++++++++- LYNA, AMAL, ENAAM -+++++++++++++++");
	char* username = getenv("USER");
	printf("\n\n\n %s@%s\n", username,getenv("PWD"));
	sleep(3);
	clear();
  // set the shell env shell=<pathname>/myshell
  setShellEnv();
}

// Function to print Current Directory.
void printDir(){
	char cwd[MAX_PATH];
	getcwd(cwd, sizeof(cwd));
	printf("\nDir: %s", cwd);
}

void remove_endOfLine(char* buf){
  int i =0;
  while(buf[i] != '\n')
    i++;
  buf[i]= '\0';
}

// Function to take input
int takeInput(char* str,int* batchFlag){

	printf("\n>>> ");
	char* buf= fgets(str,MAXCOMMAND,stdin);
    if(feof(stdin)&&batchFlag){
        printf("\nGoodbye\n");
        exit(0);
    }
  remove_endOfLine(buf);
	if (strlen(buf) != 0) {
		strcpy(str, buf);
		return 0;
	} else {
        batchFlag=0;
		return 1;
	}
}

int environ_list(void){
  char ** env = environ;
  while(*env)
   fprintf(stdout,"%s\n",*env++);
  return 0;
}

// function for finding pipe
int parsePipe(char* str, char** strpiped){
	int i;
	for (i = 0; i < 2; i++) {
		strpiped[i] = strsep(&str, "|");
		if (strpiped[i] == NULL)
			break;
	}

	if (strpiped[1] == NULL)
		return 0; // returns zero if no pipe is found.
	return 1;
}

//if '<' char was found in string inputted by user
void changeThe_inputFile(char* input_fileName){

    // fd0 is file-descriptor
    int fd0;
    if ((fd0 = open(input_fileName, O_RDONLY, 0777)) < 0) {
        perror("Couldn't open input file");
        return;
    }

    // dup2() copies content of fdo in input of preceeding file
    dup2(fd0, 0); // STDIN_FILENO here can be replaced by 0
    perror("error dup in :");
    close(fd0); // necessary
}

//if '>' char was found in string inputted by user
void changeThe_outputFileTrunc(char* output_fileName){

    int fd1 ;
    if ((fd1 = open(output_fileName ,  O_CREAT | O_TRUNC | O_RDWR, 0777)) < 0) {
        perror("Couldn't open the output file");
        return;
    }
    dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
    perror("error dup out :");
    close(fd1);
}

//if '>>' char was found in string inputted by user
void changeThe_outputFileAppend(char* output_fileName){

    int fd2 ;
    if ((fd2 = open(output_fileName ,  O_CREAT | O_APPEND | O_RDWR, 0777)) < 0) {
        perror("Couldn't open the output file");
        return;
    }

    dup2(fd2, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
    perror("error dup out :");
    close(fd2);
}

// finds where '<' or '>' occurs and remove them with the inputFileName|outputFileName
// from the ards passed to execvp method, to ensure that it wont't read them
int checkFor_IOredirection(char** parsed){

    char* parsedWithoutRedirection[MAXWORD];
    int i=0,in=0,outTrunc=0,outAppend=0,index_parsedWithoutRedirection=0;
    char inputFileName[30],outputFileName[30];

    for(i=0;parsed[i]!=NULL;i++){

        if(strcmp(parsed[i],"<")==0){
            in=1;
            i++;
            strcpy(inputFileName,parsed[i]);

        }else if(strcmp(parsed[i],">")==0){
            outTrunc=1;
            i++;
            strcpy(outputFileName,parsed[i]);

        }else if(strcmp(parsed[i],">>")==0){
            outAppend=1;
            i++;
            strcpy(outputFileName,parsed[i]);
        }else
            parsedWithoutRedirection[index_parsedWithoutRedirection++]=parsed[i];
    }

    parsedWithoutRedirection[index_parsedWithoutRedirection]=NULL;
    i=0;
    for(i=0;parsedWithoutRedirection[i]!=NULL;i++)
      strcpy(parsed[i], parsedWithoutRedirection[i]);

    parsed[i]=NULL;

    if(in)          //if '<' char was found in string inputted by user
        changeThe_inputFile(inputFileName);
    if(outTrunc)    //if '>' char was found in string inputted by user
        changeThe_outputFileTrunc(outputFileName);
    if(outAppend)   //if '>>' char was found in string inputted by user
        changeThe_outputFileAppend(outputFileName);

    if(in || outAppend || outTrunc)
        return 1;
    return 0;
}

// function for parsing command words
int parseSpace(char* str, char** parsed, int* parsedArgsLen ){
	int i;

	for (i = 0; i < MAXWORD; i++) {
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}
	*parsedArgsLen =i;
	return checkFor_IOredirection(parsed);
}

// Help command builtin
void openHelp(){
	puts("\n***WELCOME TO MY SHELL HELP***"
		"\nCopyright @ Suprotik Dey"
		"\n-Use the shell at your own risk..."
		"\nList of Commands supported:"
		"\n>cd"
		"\n>ls"
        "\n>environ"
        "\n>dir<directory>"
		"\n>quit"
        "\n>pause"
		"\n>clear"
		"\n>all other general commands available in UNIX shell"
		"\n>pipe handling"
		"\n>improper space handling");

	return;
}

// set the env  parent=<pathname>/myshell
void setParentEnv(){
  char parent[MAX_PATH];
  strcpy(parent,"parent=");
  strcat(parent,getenv("shell"));
  putenv(parent);
  return;
}

// check if there is ampersand at the end of the command line
int is_Background(char* lastStrInput) {
    if(strcmp(lastStrInput, "&") == 0) {
        return 1;
    }
    return 0;
}

// Function where the system command is executed
void execArgs(char** parsed,int* parsedArgsLen){

    int is_BG = is_Background(parsed[*parsedArgsLen-1]);
	// Forking a child
	pid_t pid = fork();

	if (pid == -1) {
		printf("\nFailed forking child..");
		return;
	} else if (pid == 0) {
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command..");
		}
		exit(0);
	} else {
        // set the env  parent=<pathname>/myshell
        setParentEnv();
		// waiting for child to terminate
		if(!is_BG )
            waitpid(pid, NULL, 0);
            is_BG=0;
		return;
	}
}

// Function where the piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe,int* parsedArgsLen){

  int is_BG = is_Background(parsed[*parsedArgsLen-1]);
  pid_t p1;
  p1 = fork();
	if (p1 < 0) {
		printf("\nCould not fork");
		return;
	}
  if (p1 == 0) {  // Child 1 "youngParent" executing..

      // 0 is read end, 1 is write end
      int pipefd[2];

      if (pipe(pipefd) < 0) {
          printf("\nPipe could not be initialized");
          return;
      }
      pid_t p2;
      p2 = fork();

      if (p2 < 0) {
          printf("\nCould not fork");
          return;
      }
      if (p2 == 0) {  // Child 2 "childOfYoungParent" executing..

          //It only needs to write at the write end
          close(pipefd[0]);
          dup2(pipefd[1], STDOUT_FILENO);
          close(pipefd[1]);

          if (execvp(parsed[0], parsed) < 0) {
              printf("\nCould not execute command 1..");
              exit(0);
          }
      }else{ // Child 1 executing.."youngParent"

          // It only needs to read at the read end
          close(pipefd[1]);
          dup2(pipefd[0], STDIN_FILENO);
          close(pipefd[0]);

          // set the env  parent=<pathname>/myshell
          setParentEnv();
          // youngParent executing, waiting for his child
          waitpid(p2, NULL, 0);

          if (execvp(parsedpipe[0], parsedpipe) < 0) {
              printf("\nCould not execute command 2..");
              exit(0);
          }
      }
  }else {
      // set the env  parent=<pathname>/myshell
      setParentEnv();
      // parent executing, waiting for his child "youngParent"
      if(!is_BG )
        waitpid(p1, NULL, 0);

      is_BG=0;
  }
}

//
int checkPathTochange (char *parsed[],char* dirpath ){

 int i=0,j=0;
 char *old_dir,*current_dir;
 DIR  *pdir;

 if(parsed[1]){       //the argument of pathname is given
    if(parsed[2]){
        fprintf(stderr,"Format Warning: invalid arguments") ;
        return -1;
    }
    dirpath[0]=0;
    if(parsed[1][0]=='~'){
          strcpy(dirpath, getenv("HOME"));
          j=strlen(dirpath);
          i=1;
    }else if(parsed[1][0]=='.'&&parsed[1][1]=='.'){
          old_dir = getenv("PWD");
          chdir("..");
          current_dir = (char *)malloc(MAX_PATH);
          if(!current_dir){
            fprintf(stderr,"invalid  allocated space" ) ;
            return -1;
          }
          getcwd(current_dir,MAX_PATH);
          strcpy(dirpath, current_dir);
          j=strlen(dirpath);
          i=2;
          chdir(old_dir);
    }else if(parsed[1][0]=='.'){
          strcpy(dirpath, getenv("PWD"));
          j=strlen(dirpath);
          i=1;
    }else if(parsed[1][0]!='/'){
          strcpy(dirpath, getenv("PWD"));
          strcat(dirpath, "/");
          j=strlen(dirpath);
      i=0;
    }
    strcat(dirpath+j,parsed[1]+i);
    printf("%s\n",dirpath);
 }

 pdir=opendir(dirpath);

 if(pdir==NULL){
    fprintf(stderr,"Path Error: \"%s\": not a directory or not exist\n",dirpath);
    return -2 ;
 }
 return 1;
}

//
void command_cd(char *parsed[]){
  char dirpath[MAX_PATH];

  if(checkPathTochange(parsed,dirpath)){
    // printf(" dir path %s\n",dirpath);
    // printf(" getenv PWD %s\n",getenv("PWD"));
    setenv("PWD",dirpath,1);
    chdir(dirpath);
    // printf(" getenv PWD %s\n",getenv("PWD"));
  }
}

//
void Command_dir(char *parsed[],int* parsedArgsLen){
	char dirpath[MAX_PATH];

  if(checkPathTochange(parsed,dirpath)){
    parsed[0]="ls";
    parsed[1]=dirpath;
    parsed[2]=NULL;
    execArgs(parsed,(int*)3) ;
  }
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed,int* parsedArgsLen){
	int NoOfOwnCmds = 8, i, switchOwnArg = 0;
	char* ListOfOwnCmds[NoOfOwnCmds];
	char* username;

	ListOfOwnCmds[0] = "quit";
	ListOfOwnCmds[1] = "cd";
	ListOfOwnCmds[2] = "help";
	ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "clear";
    ListOfOwnCmds[5] = "environ";
	ListOfOwnCmds[6] = "dir<directory>";
	ListOfOwnCmds[7] = "pause";

	for (i = 0; i < NoOfOwnCmds; i++) {
		if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
			switchOwnArg = i + 1;
			break;
		}
	}

	switch (switchOwnArg) {
	case 1:
		printf("\nGoodbye\n");
		exit(0);
	case 2:
    command_cd(parsed);
		return 1 ;
	case 3:
		openHelp();
		return 1;
	case 4:
        username = getenv("USER");
        printf("\nHello %s.\nMind that this is "
            "not a place to play around."
            "\nUse help to know more..\n",
            username);
        return 1;
    case 5:
      clear();
      return 1;
    case 6:
      environ_list();
      return 1;
    case 7:
      Command_dir(parsed,parsedArgsLen);
      return 1;
    case 8:
      getpass("press <Enter> key to continue");
      return 1;
	default:
		break;
	}

	return 0;
}

//
int checkIfThereIsABatchFile(char** parsed){

  return (strcmp(parsed[0],(char*)"myshell")==0);
}

//
int processString(char* str, char** parsed, char** parsedpipe,int* IORedirectionFlag,int* parsedArgsLen){

	char* strpiped[2];
	int piped = 0;

	piped = parsePipe(str, strpiped);

	if (piped) {
		*IORedirectionFlag=parseSpace(strpiped[0], parsed,parsedArgsLen);
		*IORedirectionFlag=parseSpace(strpiped[1], parsedpipe,parsedArgsLen);

	} else {
		*IORedirectionFlag=parseSpace(str, parsed,parsedArgsLen);
	}

  if(checkIfThereIsABatchFile(parsed))
   return 3;
  else if (ownCmdHandler(parsed,parsedArgsLen))
   return 0;
  return 1 + piped;
}

int main(){

  int stdin_copy = dup(0);
  int stdout_copy = dup(1);

	char command_inputString[MAXCOMMAND], *parsedArgs[MAXWORD];
	char* parsedArgsPiped[MAXWORD];
	int execFlag = 0,batchFlag=0,IORedirectionFlag=0,parsedArgsLen=0;

	init_shell();

	while (1) {
//        printf("test: %s ",getenv("PWD"));
		// print shell line
		printDir();
		// take input
		if (takeInput(command_inputString,&batchFlag))
			continue;
		// process
		execFlag = processString(command_inputString,parsedArgs, parsedArgsPiped,&IORedirectionFlag,&parsedArgsLen);
		// execflag returns zero if there is no command
		// or it is a builtin command,
		// 1 if it is a simple command,
		// 2 if it is including a pipe,
        // 3 if it is reading from a batchFile.

		// execute
		if (execFlag == 1)
			execArgs(parsedArgs,&parsedArgsLen);

		if (execFlag == 2)
			execArgsPiped(parsedArgs, parsedArgsPiped,&parsedArgsLen);

        if (execFlag == 3){
            batchFlag=1;
            changeThe_inputFile(parsedArgs[1]);
        }

        if(IORedirectionFlag){
            // reinialize the I/O redirection to the default values
            dup2(stdin_copy, 0);
            dup2(stdout_copy, 1);
        }
    }
    close(stdin_copy);
    close(stdout_copy);
	return 0;
}
