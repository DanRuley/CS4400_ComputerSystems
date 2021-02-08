#include "csapp.h"

void hit(int sigchld) {
  static int hits = 0;
  if (++hits == 9) _exit(0);

}

static char *const argv[] = { "/bin/cat", NULL };

int main() {
  pid_t pid;
  Signal(SIGINT, hit);
  pid = Fork();
  if (pid == 0) {
    Setpgid(0,0);
    Execve(argv[0], argv, NULL);

  }

  Waitpid(pid, NULL, 0);
  return 0;
}
