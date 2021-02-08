#include <stdio.h>

void print_binary_char(unsigned char n){
  int i;
  for(i = (sizeof(unsigned char) * 8) - 1; i >= 0; i--){
    printf("%d", (n >> i) & 1);
    if(i % 8 == 0)
      putchar(' ');
  }
  putchar('\n');
}

void print_binary_short(unsigned short n){
  int i;
  for(i = (sizeof(unsigned short) * 8) - 1; i >= 0; i--){
    printf("%d", (n >> i) & 1);
    if(i % 8 == 0)
      putchar(' ');
  }
  putchar('\n');
}

void print_binary_int(unsigned int n){
  int i;
  for(i = (sizeof(unsigned int) * 8) - 1; i >= 0; i--){
    printf("%d",(n >> i) & 1);
    if(i % 8 == 0)
      putchar(' ');
  }
  putchar('\n');
}

void print_binary_long(unsigned long n){
  int i;
  for(i = (sizeof(unsigned long) * 8) - 1; i >= 0; i--){
    printf("%d", (n >> i) & 1);
    if(i % 8 == 0)
      putchar(' ');
  }
  putchar('\n');
}
/*
int main(void){

  char c = 0xd3;
  short s = 0xff1a;
  int i = 0xabcd1234;
  long l = 0xabcd1234abcd1234;

  print_binary_char(c);
  print_binary_short(s);
  print_binary_int(i);
  print_binary_long(l);
    
  return 0;
*/


