#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> // for strtok()
#include <ctype.h> // for isspace()
#include <unistd.h> // for execvp()
#include <sys/wait.h> // for open()
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>
#include <stdbool.h> // for booleans

//#define MAX_CMD_LENGTH 513;

void parseCommand(char *cmd);
void skipLine(FILE *stream);
char* removeNewline(char* arg);

void printError() {
  char error_message[30] = "An error has occurred\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

 char* prompt = "mysh>";
 char* newline = "\n";
 
int main(int argc, char** argv) {

  if (argc > 2) {
    printError();
    exit(1);
  }
  char cmd[1024]; // str array where input is stored
  // interactive mode
  if (argc == 1) {
    while(1) {
      printf(prompt);
      fgets(cmd, sizeof(cmd), stdin);

      // check for input too long
      int i;
      bool isTooLong = true;
      for (i = 0; i < 513; i++) {
	if (cmd[i] == '\n') {
	  isTooLong = false;
	}
      }
      // if too long print error and get next input
      if (isTooLong == 1) {
	printError();
	continue;
      }
      parseCommand(cmd);
    }
  }

  // batch mode
  else if (argc == 2) {
    // open input file
    char* batchFile = strdup(argv[1]);
    FILE *fp = fopen(batchFile, "r"); 
    if (fp == NULL) {
      printError();
      exit(1);
    }
      
    // while more non-EOF chars, keep processing
    while (fgets(cmd, sizeof(cmd), fp) != NULL) { 
      if (ferror(fp) != 0) {
	// if error when reading from file
	printError();
	exit(0);
      }
      // print out command
      write(STDOUT_FILENO, cmd, strlen(cmd));
      // check if input is too long
      bool isTooLong = true;
      int i;
      for (i = 0; i < 513; i++) {
	if (cmd[i] == '\n') {
	  isTooLong = false;
	  break;
	}
      }
      // if too long print error and get next input
      if (isTooLong) {
	printError();
	continue;
      }
     // process command
      parseCommand(cmd);
     }
  }

  return 0;
}



void parseCommand(char *cmd) {
  // most possible tokens will be 512/2 = 256
  char *allTokens[256];
  char* token;
  int status = 0;
  // whitespace in command should be ignored
  char delimiters[] = " \n\t\f\r\v";
  
  // read in first whitespace delimited token
  token = strtok(cmd, delimiters);
  int tokenCount = 0;
  bool isArrow = false;
  char* outFile;

  // read in remaining tokens and place into allTokens array
  while (token != NULL && tokenCount < 256) {
    // check for weird line feed case -- not a robust solution
    if (token[0] == 10) {
      // if it is a line feed, don't process it
      break;
    }
    // take note of > char, but don't include it in args
    if (token[0] == '>') {
      isArrow = true;
      token = strtok(NULL, " ");
      continue;
    }
    if (isArrow) {
      // if arrow char is found, next token is outfile
      outFile = token;
      if (outFile[strlen(outFile) - 1] == '\n') {
	// replace with blank
	outFile[strlen(outFile) - 1] = 0;
      }
      token = strtok(NULL, " ");
      // should not be any more args!
      if (token != NULL) {
	printError();  // too many args after >
	return;
      }
      continue;
    }
    allTokens[tokenCount] = token;
    tokenCount++;
    // NULL tells it to resume from last spot in string
    token = strtok(NULL, " ");
  }

  // place args into NULL terminated array of correct length
  char *args[tokenCount + 1];
  int i;
  for (i = 0; i < tokenCount; i++) {
    int len = strlen(allTokens[i]);
    // if last char is a newline, strip it away
    if (allTokens[i][len - 1] == '\n') {
      allTokens[i][len - 1] = 0;
    }
    args[i] = allTokens[i];
    //printf("arg %d: %s\n", i, args[i]);
  }
  args[tokenCount] = NULL;
  int numElements = (sizeof(args)/sizeof(args[0]));
  



  // execute program based on first argument
  if (args[0] == NULL) {
    ; // nothing entered
  } 

  // exit
  else if (strcmp(args[0], "exit") == 0) {
    if (numElements == 2) {
      exit(0);
    } else {
      printError();
    }
  }

  // change directory -- WORKS
  else if (strcmp(args[0], "cd") == 0) {
    int cdRet;
    // no directory specified (cd + NULL)
   if (numElements == 2) {
      cdRet = chdir(getenv("HOME"));
      if (cdRet == -1) {
	printError(); // bad path
      }
   } else if (numElements == 3) {
     cdRet = chdir(args[1]);
     if (cdRet == -1) {
       printError(); // bad path
     }
   } else {
     printError(); // incorrect # of args
   }
  }

  // print working directory
  else if (strcmp(args[0], "pwd") == 0) {
    if (numElements == 2) {
      char buffer[512];
      getcwd(buffer, sizeof(buffer));
      write(STDOUT_FILENO, buffer, strlen(buffer));
      write(STDOUT_FILENO, newline, strlen(newline));
      } else {
      printError();
    }
  }

  // other commands
  else {
    int i;
    for (i = 0; i < numElements - 1; i++) {
      ;//printf("%s\n", args[i]);
    }
    int rc = fork();
    // if not the parent process
    if (rc == 0) {
      // handle output redirection
      if (isArrow) {
	// if second to last arg is > there is redirection
	close(STDOUT_FILENO); // no writing to screen
	int openResults = open(outFile, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	if (openResults < 0) {
	  // error occurred opening file
	  printError();
	  return;
	}
      }
      execvp(args[0], args);
      printError(); // return from execvp means error -- incorrect command
    } else {
      wait(&status);
    }
  } 
  return;
 }

/* Takes a pointer to an input file and reads in characters
 * until it encounters a '\n' or the end of the file.  Used to
 * skip the remainder of a line/command that is too long.
 */
void skipLine(FILE *stream) {
  int ch;
  // read in all non \n and non EOF characters
  while ((ch = getc(stream)) != EOF && ch != '\n') {
    printf("%c\n", ch);
  }
  return;
}

char* removeNewline(char* arg) {
  int  numChars = (sizeof(arg))/(sizeof(arg[0]));
  char* retArg[numChars - 1];
  int i = 0;
  for (i = 0; i < numChars - 1; i++) {
    *retArg[i] = arg[i];
  }
  return retArg[0];
}
