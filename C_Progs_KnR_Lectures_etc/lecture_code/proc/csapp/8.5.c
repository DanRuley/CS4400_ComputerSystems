#include <stdio.h>

unsigned int wakeup(int sec){
  sleep(sec);
  printf("woke up after %d sec\n", sec);
}


int main(){
  
  wakeup(2);
}
