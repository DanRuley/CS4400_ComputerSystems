#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  
  int parent = getpid();
  //printf("%d %d %d\n", getpid(), fork(), getppid());
  int firstchild = fork();
  

  if(firstchild == 0) {
    
    /* Target 1
    if(fork() != 0)
      fork();
    */


    /* Target 2
    if(fork() == 0)
      if(fork() != 0)
	fork();
    */

    /* Target 3
    if (fork() != 0)
      if (fork() != 0)
	fork();
      else
	fork();
    else
      if(fork() != 0)
	fork();
      else
	fork();
    */
  
    if (fork() != 0) {
      if(fork() == 0) {
        fork();
	fork();
      }
    }
    else {
      fork();
      if(fork() == 0) {
        fork();
      } 
    }


    // Do work here to fork to create the target pstree diagram.
    //fork();
  
    // Put all calls to fork() above this line, and don't modify the code below.
  
    sleep(2); /* required so that execve runs before the process quits (don't do this sort of thing in real code) */
    return 0;
  }
  
  else {
    sleep(1); /* ensure child finishes forking before parent calls execve */


    // The parent process calls execve on pstree.

    char pid[20]; // location to put the pid argument to the pstree execution
    char* args[] = {"/usr/bin/pstree", "-p" , pid, NULL};
    char* environ[] = {NULL};

    sprintf(pid, "%u", firstchild);
    execve( args[0], args, environ);
  }
}
