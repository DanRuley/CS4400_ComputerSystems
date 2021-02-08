#include "csapp.h"

static void ack(int sigchld) {
  printf("got alarm\n");
}

int main() {
  Signal(SIGALRM, ack);

  if(Fork() == 0) {
    while(1)
      Kill(getppid(), SIGALRM);
  } else {
    double a = 1.0;
    while(1) {
      printf("%.2f ", a);
      a = a + 1.0;
    }
  }
}
