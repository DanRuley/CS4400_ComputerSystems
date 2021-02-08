#include <stdio.h>

long iterate(long v, int steps)
{
  while (steps--) {
    if (v & 0x1) {
      v = 3*v + 1;
      printf("%ld ",v);
    } else {
      printf("%ld ",v);
      v = v >> 1;
    }
  }

  return v;
}

int main(){
  iterate(13, 100);

}
