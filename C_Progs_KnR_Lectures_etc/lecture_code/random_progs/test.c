#include <stdio.h>
#

typedef char* word;

void update(int arr[2]) {
  printf("%x  arr[2]:%x   arr[4]:%x\n",arr,&arr[2],&arr[4]);
  arr[4]++;
}

void f (int arr[][30], int* p)
{
  printf("*p: %d, *(p + 4): %d\n", *p, *(p+4));
}

int main() {
  int arr[20][30];
  int* ptr = &arr[3][29];
  *ptr = 5;
  arr[3][33] = 19;
  printf("%x    %x\n",ptr, (ptr + 4));
  printf("%x    %x\n",&arr[3][29], &arr[4][3]);

  f(arr, ptr);
  
}

