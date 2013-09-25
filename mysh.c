#include <stdio.h>
#include <ctype.h> // for isspace()

char** parseCommand(const char *cmd);

int main(int argc, char** argv) {
  printf("%d\n", argc);

  char cmd[512]; // str array where input is stored

  while(1) {
    printf("mysh> ");
    fgets(cmd, 512, stdin);
    printf("mysh> ");
    parseCommand(cmd);
  }
  return 0;
}

char** parseCommand(const char *cmd) {
  //char* parsed[512];
  //char tmp[512];
  int i = 0;
  while(*(cmd + i) != ' ') {
    putc(cmd[i], stdout);
    i++;
  }
  return NULL;
}
