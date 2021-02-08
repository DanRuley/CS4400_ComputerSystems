#include <unistd.h>

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

int mm_init() {
  return 0;
}

void *mm_malloc(size_t size) {
  return sbrk(ALIGN(size));
}

void mm_free(void *p) {
}
