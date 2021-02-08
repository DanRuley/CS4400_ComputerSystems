#include "csapp.h"

#define ITERS 1000000

int main(){

  int fds[2];
  int i;

  Pipe (fds);
  if (Fork() == 0) {
    FILE *out = fdopen(fds[1], "w");
    for (i = 0; i < ITERS; i++)
      fwrite("Hello", 1, 5, out);
  } else {
    FILE *in = fdopen(fds[0], "r");
    char buffer[5];
    int n = 0;
    for (i = 0; i < ITERS; i++)
      n+= fread(buffer, 1, 5, in);
    printf("%d\n", n);
  }
  return 0;
}
