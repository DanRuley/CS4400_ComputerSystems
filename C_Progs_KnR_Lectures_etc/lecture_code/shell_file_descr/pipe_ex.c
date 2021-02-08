#include <unistd.h>
#include "csapp.h"

int main(){

  int fds[2];
 

  Pipe(fds);

  if(Fork() == 0){
    Sleep(1);
    Write(fds[1], "Hello", 5);
  } else {
    char buf[6];   
    Read(fds[0], buf, 5);
    buf[5] = 0;
    printf("%s\n", buf);
    }
  
  return 0;

}
