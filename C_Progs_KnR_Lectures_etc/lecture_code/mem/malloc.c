#include <stdlib.h>
#include <stdio.h>

int main(){
  int *i = (int*) malloc(10 * sizeof(int));
  int j;
  for(j = 0; j < 10; j++)
    i[j] = j + 1;

  i = (int*) realloc(i, sizeof(int) * 20);
  for(j = 0; j < 20; j++)
    printf("%d ", i[j]);
  printf("\n");


}
