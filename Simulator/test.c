#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static void print_binary(size_t n){
  int i = sizeof(size_t) * 8 - 1;
  while(!((n >> i) & 1))
    i--;
  for(; i >= 0; i--)
    putchar(((n >> i) & 1) + '0');
  putchar('\n');
}


int main(){
  print_binary(42);
  print_binary(-3673283);
	     

}
