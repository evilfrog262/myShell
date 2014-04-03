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

void printError() {
  char error_message[30] = "An error has occured\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}

 char* prompt = "mysh>";
 char* newline = "\n";
 
int main(int argc, char** argv) {

  char cmd[513]; // str array where input is stored
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
      exit(0);
    }
      
    // while more non-EOF chars, keep processing
    write(STDOUT_FILENO, prompt, strlen(prompt));
    //fgets(cmd, sizeof(cmd), fp);
    while (fgets(cmd, sizeof(cmd), fp) != NULL) { 
      if (ferror(fp) != 0) {
	// if error when reading from file
	printError();
	exit(0);
      }
      // print out command
      write(STDOUT_FILENO, cmd, strlen(cmd));
      write(STDOUT_FILENO, newline, strlen(newline));
      // check if input is too long
      bool isTooLong = true;
      int i;
      for (i = 0; i < 513; i++) {
	if (cmd[i] == '\n') {
	  isTooLong = false;
	}
      }
      // if too long print error and get next input
      if (isTooLong) {
	printError();
	// continue to read characters until a \n is encountered
	skipLine(fp);
	continue;
      }
      // process command
      parseCommand(cmd);
      // re-write prompt
      write(STDOUT_FILENO, prompt, strlen(prompt));
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
  char delimiters[3];
  delimiters[0] = ' ';
  delimiters[1] = '\n';
  delimiters[2] = '\t';

  // read in first whitespace delimited token
  token = strtok(cmd, delimiters);
  int tokenCount = 0;

  // read in remaining tokens and place into allTokens array
  while (token != NULL) {
    allTokens[tokenCount] = token;
    tokenCount++;
    // NULL tells it to resume from last spot in string
    token = strtok(NULL, " ");
  }

  // place args into NULL terminated array of correct length
  char *args[tokenCount + 1];
  int i;
  for (i = 0; i < tokenCount; i++) {
    args[i] = allTokens[i];
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

  // change directory -- SORT OF WORKS
  else if (strcmp(args[0], "cd") == 0) {
    // no directory specified (cd + NULL)
   if (numElements == 2) {
      chdir(getenv("HOME"));
   } else if (numElements == 3) {
     printf("%s\n", args[1]);
     chdir(args[1]);
   } else {
     printError();
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
    int rc = fork();
    // if not the parent process
    if (rc == 0) {
      // handle output redirection
      if (strcmp(args[numElements - 3], ">") == 0) {
	// if second to last arg is > there is redirection
	close(STDOUT_FILENO); // no writing to screen
	open(args[numElements - 2], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
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
    ;
  }
  return;
}
