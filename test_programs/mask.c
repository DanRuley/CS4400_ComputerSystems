#include <stdio.h>

int num_bits(int x)
{
  int ret = 0;
  while (x != 0){
    x = x >> 1;
    ret += x == 0 ? 0 : 1;
  }
  return ret;
}

void print_binary(int x){
  unsigned int mask;
  int i;
  for(i = num_bits(x); i >= 0; i--){
    mask = 1 << i;
    putchar(((x & mask) >> i) + '0');
  }
}


int main(){
  
  unsigned char mask = 0x3f;
  unsigned int bits = 0xAABBCCDD;
    
  unsigned char leastsig = 0x03;
  leastsig &= bits;
  printf("%x\n",leastsig);
  print_binary(leastsig);
  putchar('\n');

  return 0;
}
