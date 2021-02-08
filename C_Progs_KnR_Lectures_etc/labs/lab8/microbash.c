#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
#define STDIN_FILENO    0      
#define STDOUT_FILENO   1       
#define STDERR_FILENO   2       
*/

int main(int argc, char **argv) {

  int i;
  for(i = 0; i < argc; i++)
    printf("argv[%d]: %s \n", i, argv[i]);
  char* writer_args[] = {argv[1], NULL};
  char* rw_args[] = {argv[2], NULL};
  char* reader_args[] = {argv[3], NULL};
  char* env[] = {NULL};
  int fds[4];
  
  pipe(fds);
  printf("fds: [%d, %d, %d, %d]\n", fds[0], fds[1], fds[2], fds[3]);
  pipe(fds + 2);
  printf("fds: [%d, %d, %d, %d](after 2nd pipe call)\n", fds[0], fds[1], fds[2], fds[3]); 
   
  // First child --> output of execve now writes to fds[1] file instead of stdout
  if(fork() == 0) {
    close(fds[0]);
    close(fds[2]);
    close(fds[3]);
    dup2(fds[1], STDOUT_FILENO);
    if(execve(writer_args[0], writer_args, env) == -1)
      exit(1);
    close(fds[1]);
  }
  
  // Second child -> input to execve now comes from fds[0] read end instead of stdin.
  if(fork() == 0) {
    close(fds[2]);
    close(fds[1]);
    dup2(fds[0], STDIN_FILENO);
    dup2(fds[3], STDOUT_FILENO);
    if(execve(rw_args[0], rw_args, env) == -1)
      exit(1);
    close(fds[0]);
    close(fds[3]);  
  }
  else {
    if (fork() == 0){
      close(fds[0]);
      close(fds[1]);
      close(fds[3]);
      dup2(fds[2], STDIN_FILENO);
      if(execve(reader_args[0], reader_args, env) == -1)
	exit(1);
      close(fds[2]);
    }

  }
  
  

  
  close(fds[0]);
  close(fds[1]);
  close(fds[3]);
  close(fds[4]);


  // Parent
  int status;
  while(waitpid(-1, &status, 0) > 0)
    ;
}

