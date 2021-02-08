#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int x = 10;
void handler(sig) {
  x += 3;
}

int main() {


  printf("a\n");
  

  if(fork() == 0) {
    printf("b\n");
    if(fork() == 0){
      exit(0);
    }
    else{
      printf("c\n");
    }
  }
  else
    wait(NULL);

  printf("d\n");
  return 0;

}
