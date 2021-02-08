#include <unistd.h>
#include <stdio.h>

int main() { 
  int pid1;
  int pid2;
  int pid3;
  int pid4;

  pid1 = getpid();
  pid2 = fork();
  pid3 = fork();
  pid4 = getppid();
 
  if(pid1 == pid2) 
    printf("u"); 
  if(pid1 == pid3) 
    printf("v"); 
  if(pid1 == pid4) 
    printf("w"); 
  if(pid2 == pid3) 
    printf("x"); 
  if(pid2 == pid4) 
    printf("y"); 
  if(pid3 == pid4) 
    printf("z");

  sleep(1);   // simply pauses execution, does not affect what is printed
}
