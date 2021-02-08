#include <unistd.h>
#include <stdio.h>

int main() {
  printf("%d\n", getppid());
  while(1) {
    fork();
    sleep(1);

}
}
