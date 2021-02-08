#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t pid = fork();
  if(pid == 0)
    exit(42);
  else {
    int status = 0;
    waitpid(pid, &status, 0);
    printf("%d", WEXITSTATUS(status));
  }
}
