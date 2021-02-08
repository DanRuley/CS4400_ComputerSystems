#include <stdio.h>
#include <string.h>
#define MAXLINE 1000

int main() {
  char buf[MAXLINE];

  while(fgets(buf, MAXLINE, stdin)) {
    char *token;
    token = strtok(buf, " ");
    while(token != NULL) {
      printf("%s\n", token);
      token = strtok(NULL, " ");
    }

    //printf("%s",buf);
  }
  return 0;
}
