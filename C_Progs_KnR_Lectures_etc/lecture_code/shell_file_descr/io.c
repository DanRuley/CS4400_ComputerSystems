#include "csapp.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main() {

  pid_t pid;
  int fds[2], n;


  //fds 0: read end of pipe, fds 1: write end of pipe
  Pipe(fds);

  pid = Fork();

  if (pid == 0) {
    //redirect STD out to write to fds[1] instead
    //ie change output port to write to output PIPE
    Dup2(fds[1], STDOUT);
    printf("Hello!");
  }else {
    char buffer[32];
    Close(fds[1]);
    Waitpid(pid, NULL, 0);
    n = Read(fds[0], buffer, 31);
    buffer[n] = 0;
    printf("Got: %s\n", buffer);
  }
  return 0;
}
