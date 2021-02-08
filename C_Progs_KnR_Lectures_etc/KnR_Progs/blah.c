#include <stdio.h>
#include <stddef.h>

typedef struct str{
  double x;
  double y;
}str;


int main(){
  int i = 0x746162;
  char* str = (char*)&i;
  printf("%s",str);
}
