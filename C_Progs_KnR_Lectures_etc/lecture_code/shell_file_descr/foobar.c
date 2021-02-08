#include "csapp.h"

int main(){
  int fds[2];
  char c;
  fds[0] = Open("foobar.txt", O_RDONLY, 0);
  fds[1] = Open("foobar.txt", O_RDONLY, 0);
  printf("%d %d\n", fds[0], fds[1]);
  Read(fds[1], &c, 1);
  Read(fds[1], &c, 1);
  Read(fds[1], &c, 1);  

  Dup2(fds[1], fds[0]);
  Read (fds[0], &c, 1);
  printf("c = %c\n", c);
  exit(0);
}
