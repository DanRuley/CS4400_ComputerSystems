#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"

unsigned int wakeup(unsigned int secs){
  unsigned int s = sleep(secs);
  printf("Woke up at %d sec.\n", s);
  return s;
}
extern char** environ;

int main(){
  
  char f[] = "/usr/bin/wc";
  char *args[] = {"/usr/bin/wc", "waitpid2.c", NULL};
  //char *env[] = NULL;

  int i;
  i = execve(f, args, environ);

  printf("If you're reading this, something went terribly wrong %d.\n", i);
}
