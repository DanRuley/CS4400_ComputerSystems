#include "csapp.h"

static char *s;
static size_t sz = 1 << 14;

static void recover(int sig) {
  sio_puts("ouch...\n");
  Mprotect(s, sz, PROT_READ | PROT_WRITE);
}

int main() {
  s = Mmap(0, sz, PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
  Signal(SIGSEGV, recover);

  s[0] = 0xff;
  printf("ok after all!\n%x",s[0]);
}
