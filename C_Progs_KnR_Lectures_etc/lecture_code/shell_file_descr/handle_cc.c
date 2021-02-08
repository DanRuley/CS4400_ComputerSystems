#include "csapp.h"
#include <signal.h>

void handle(int sigint) {
  printf("received signal: %d\n", sigint);
}

int main(){
  Signal(SIGTSTP, handle);
  sigset_t sigs;
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGTSTP);


  while(1){
    printf("tick...\n");
    sigprocmask(SIG_BLOCK, &sigs, NULL);
    Sleep(2);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
  }



}
