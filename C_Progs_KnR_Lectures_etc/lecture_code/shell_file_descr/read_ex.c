#include "csapp.h"

int main(int argc, char **argv){
  int fd = Open(argv[0], O_RDONLY, 0);
  char buf[5];

  Read(fd, buf, 4);
  buf[4] = 0;
  
  printf("%s\n", buf);
  return 0;

}
