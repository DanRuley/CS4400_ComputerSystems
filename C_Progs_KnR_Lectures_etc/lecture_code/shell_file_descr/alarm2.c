#include "csapp.h"

static void ack(int sigchld) {
  /* broken(without the commented code); sets errno to ECHILD: */
  //int save_errno = errno;
  waitpid(getpid(), NULL, 0);
  //errno = save_errno;
}

int main() {
  Signal(SIGALRM, ack);

  if(Fork() == 0) {
    while(1)
      Kill(getppid(), SIGALRM);
  } else {
    
    while(1) {
      /*broken; should set errno to ENOENT */
      open("not_there.txt", O_RDONLY);
      if (errno == ECHILD)
	printf("ECHILD from open?!\n");
      else printf("%d\n", errno);
    }
  }
}
