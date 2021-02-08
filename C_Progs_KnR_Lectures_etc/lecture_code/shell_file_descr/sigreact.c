#include "csapp.h"

static volatile int child_running = 0;
static volatile pid_t pid;

void done(int sigchld) {
  int status;
  sio_putl(pid);
  sio_puts("\n");
  Waitpid(pid, &status, 0);
  child_running = 0;
}

int main() {

  sigset_t sigs;
  Sigemptyset(&sigs);
  Sigaddset(&sigs, SIGCHLD);

  //register done as a handler for the SIGCHLD signal.
  Signal(SIGCHLD, done);

  while(1){
    if (!child_running) {
      child_running = 1;
      sigprocmask(SIG_BLOCK, &sigs, NULL);
      pid = Fork();
      sigprocmask(SIG_UNBLOCK, &sigs, NULL);
      if (pid == 0)
	return 0;
    }
    //printf("Tick... \n");
    //Sleep(1);
  }
}
