#include <stdio.h>

int foo(int n){
  if(n == 0)
    return 1;

  return n * foo(n-1);
}

void main(){
  unsigned int f = foo(3);

  printf("%u", f);

}

