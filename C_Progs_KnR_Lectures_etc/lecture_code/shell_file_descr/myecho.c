#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(char *f, char **argp, char **envp){
  char *temp;
  int i = 0;
  while((temp = *(argp + i)) != NULL)
    printf("args[%d]: %s\n", i++, temp);

  while((temp = *(envp + i)) != NULL)
    printf("env[%d]: %s\n", i++, temp);
}
