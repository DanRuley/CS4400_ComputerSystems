#include <stdio.h>

int num_bits(int x)
{
  int ret = 0;
  while (x != 0){
    x = x >> 2;
    ret++;
  }
  return ret + 1;
}

void print_binary(int x){
  unsigned int mask;
  int i;
  for(i = 7; i >= 0; i--){
    mask = 1 << i;
    putchar(((x & mask) >> i) + '0');
  }
}


int main(){
  int i;
  for(i = 1; i < 256; i++)
    {
      printf("dec: %d  bin: ", i);
      print_binary(i);
      printf("%s", (i % 5 == 0) ? "\n" : " ");
    }
  printf("\n");

  return 0;
}
