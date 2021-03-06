#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> // for strtok()
#include <ctype.h> // for isspace()
#include <unistd.h> // for execvp()

void parseCommand(char *cmd);

void printError() {
  char error_message[30] = "An error has occured\n";
  write(STDERR_FILENO, error_message, strlen(error_message));
}


int main(int argc, char** argv) {

  char cmd[512]; // str array where input is stored
  
  while(1) {
    printf("mysh > ");
    fgets(cmd, 512, stdin);
    parseCommand(cmd);
  }
  return 0;
}



void parseCommand(char *cmd) {
  // most possible tokens will be 512/2 = 256
  char *allTokens[256];
  char* token;
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



  // execute program based on first argument
  if (args[0] == NULL) {
    printf("No command was entered\n");
  } 

  // exit
  else if (strcmp(args[0], "exit") == 0) {
    exit(0);
  }

  // change directory
  else if (strcmp(args[0], "cd") == 0) {
    printf("cd\n");
    // no directory specified
    if (sizeof(args) == 1) {
      chdir(getenv("HOME"));
    } else {
      chdir(args[1]);
    }
  }

  // print working directory
  else if (strcmp(args[0], "pwd") == 0) {
    printf("pwd\n");
    char buffer[512];
    getcwd(buffer, sizeof(buffer));
    printf("%s\n", buffer);
  }

  else if (strcmp(args[0], "ls") == 0) {
    printf("ls\n");
    int rc = fork();
    // if not the parent process
    if (rc > 0) {
      char* file = "/bin/";
      execvp(file, args);
      printf("Execvp returned --> fail\n");
    }  
  }
 }
