#include <unistd.h>
#include <sys/mman.h>

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define CHUNK_SIZE (1 << 14)
#define CHUNK_ALIGN(size) (((size) + (CHUNK_SIZE - 1)) & ~(CHUNK_SIZE - 1))

#define OVERHEAD sizeof(block_header);

void *current_avail = NULL;
size_t current_avail_size = 0;

typedef struct{
  size_t size;
  char allocated;
} block_header;


int mm_init(){
  current_avail = sbrk(0);
  current_avail_size = 0;
  return 0;
}

void *mm_malloc(size_t size){
  size_t newsize = ALIGN(size);
  void *p;
  
  if(current_avail_size < newsize) {
    current_avail = mmap(0, CHUNK_ALIGN(newsize), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    current_avail_size = CHUNK_ALIGN(newsize);
  }

  p = current_avail;
  current_avail += newsize;
  current_avail_size -= newsize;

  return p;

}

void mm_free(void *p){

}
