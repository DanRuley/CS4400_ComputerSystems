#include "csapp.h"

#define ITERS 1000000

int main(){

  int fds[2];
  int i;

  Pipe (fds);
  if (Fork() == 0) {
    for (i = 0; i < ITERS; i++)
      Write(fds[1], "Hello", 5);
  } else {
    char buffer[5];
    int n = 0;
    for (i = 0; i < ITERS; i++)
      n+= Read(fds[0], buffer, 5);
    printf("%d\n", n);
  }
  return 0;
}
