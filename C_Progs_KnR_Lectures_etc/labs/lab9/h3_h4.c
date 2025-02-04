#include <stdio.h>
#include <stdlib.h>

#define N 10

static int *matrix_times_vector(int A[N][N], int *x) {
  int *y = malloc(N * sizeof(int));
  int i, j;
  for(i = 0; i < N; i++)
    y[i] = 0;

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
      y[i] += A[i][j] * x[j];
  return y;
}

static void show_vector(int *y){
  int i;
  for(i = 0; i < N; i++)
    printf("%d%s", y[i], i == N - 1 ? "" : " ");
  printf("\n");
}



int main(){
  int A[N][N] = {{0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9},
		 {0,1,2,3,4,5,6,7,8,9}};
  int x[N] = {10, 100, 1000, 10000, 1000, 10, 100, 1000, 10000, 1000};

  int *y1;
  int *y2;
  int i;
  for(i = 0; i < 10000; i++){
  
  y1 = matrix_times_vector(A, x);
  y2 = matrix_times_vector(A, x);
  show_vector(y1);
  show_vector(y2);
  free(y1);
  free(y2);
  }
 

}
