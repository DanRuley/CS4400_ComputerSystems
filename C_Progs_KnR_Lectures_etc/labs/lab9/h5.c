#include <stdio.h>
#include <stdlib.h>
#define MAXIN 8

int main() {
  //MAXIN + 1 b/c null terminated
  char s[MAXIN + 1];

  fgets(s, MAXIN, stdin);

  printf("Got %s\n", s);

  return 0;

}
