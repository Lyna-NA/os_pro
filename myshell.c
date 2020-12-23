#include "myshell.h"


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
            if(dup2(stdin_copy, 0)<0)
                perror("ERROR, dup in :");
            if(dup2(stdout_copy, 1)<0)
                perror("ERROR, dup out :");
        }
        if(batchFlag)
            sleep(2);
    }
    close(stdin_copy);
    close(stdout_copy);
	return 0;
}
