#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>

int main(){
  int **n;
  n = (int**) malloc(5000000 * sizeof(int*));

  int count = 0;
  struct timeval start_mal, stop_mal, start_cal, stop_cal;
  long long total_mem = 0;
  gettimeofday(&start_mal, NULL);
  while(total_mem / 1000000 < 20000){
    n[count++] = (int*) calloc(1000, sizeof(int));
    total_mem += 1000*sizeof(int);
  }
  gettimeofday(&stop_mal, NULL);

  int i;
  for(i = 0; i < count; i++)
    free(n[i]);

  count = 0;
  gettimeofday(&start_cal, NULL);
  while(total_mem / 1000000 < 20000){
    n[count++] = (int*) calloc(1000, sizeof(int));
    total_mem += 1000*sizeof(int);
  }
  gettimeofday(&stop_cal, NULL);

  for(i = 0; i < count; i++)
    free(n[i]);

  free(n);

  printf("malloc took %lu sec\n", stop_mal.tv_sec - start_mal.tv_sec);

  printf("calloc took %lu sec\n", stop_cal.tv_sec - start_cal.tv_sec);
}
