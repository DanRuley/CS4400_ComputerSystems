#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
#define STDIN_FILENO    0       /* Standard input.  */
#define STDOUT_FILENO   1       /* Standard output.  */
#define STDERR_FILENO   2       /* Standard error output.  */
*/

int main(){
  const char* str = "This is a nice long string. I wonder how many words are in it?";

  char* counter_args[] = {"/usr/bin/wc", NULL};
  char* env[] = {NULL};
  
  int fd[2];

  //pipe created
  pipe(fd);


  if(fork() == 0){
    //child needs read end of pipe (fd[0])
    //child needs to close write end
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    if(execve(counter_args[0], counter_args, env) == -1)
      exit(1);
  }

  printf("%s\n", str);

  //parent needs to close the read end
  close(fd[0]);

  dprintf(fd[1], "%s", str);
  close(fd[1]);
  wait(NULL);
}
