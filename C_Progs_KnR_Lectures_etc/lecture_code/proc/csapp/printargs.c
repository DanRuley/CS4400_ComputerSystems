#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp)
{
  int i;
  for(i = 0; i < argc; i++)
    printf("argv[%d]: %s\n", i, argv[i]);
  
  i = 0;
  while(envp[i] != NULL){
    printf("envp[%d]: %s\n", i, envp[i]);
    i++;
  }

  exit(0);
}
