#include "csapp.h"

/* Wait() is the same as Waitpid() with the 
   pid and options parameters set to defaults.
   That is, it blocks until any child terminates. */

int main() {
  if(Fork() == 0) {
    if(Fork() == 0) 
      printf("a");
    else {
      pid_t pid;
      int status;
      if((pid = Wait(&status)) > 0) 
        printf("b");
    }
  }
  else {
    printf("c");
    exit(0);
  }
  printf("d");
  return 0;
}
