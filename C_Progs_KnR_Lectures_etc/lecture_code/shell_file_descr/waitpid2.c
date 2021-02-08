#include "csapp.h"
#define N 100

int main() {
  int status, i;

  pid_t pid[N], retpid;

  for(i = 0; i < N; i++)
    if((pid[i] = Fork()) == 0)
      exit(100 + i);

  i = 0;
  while((retpid = waitpid(pid[i++], &status, 0)) > 0) {

      if(WIFEXITED(status))
	printf("Process %d exited normally with exit status %d.\n",retpid, WEXITSTATUS(status));
      else
	printf("Process %d exited abnormally.\n",retpid);
    }
      
    if (errno != ECHILD)
      printf("something went horribly wrong because waitpid returned an error other than no more children.\n");

    return 0;

}
