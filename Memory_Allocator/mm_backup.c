/*
 * mm-naive.c - The least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by allocating a
 * new page as needed.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

typedef struct free_list{
  struct free_list *prev;
  struct free_list *next;
}free_list;

typedef size_t block_header;
typedef size_t block_footer;

#define PAGESIZE 4096
#define OVERHEAD (sizeof(block_header)+sizeof(block_footer))
#define PAGE_OVERHEAD (sizeof(block_header) * 4)
#define ALIGNMENT 16
#define BYTE 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/* rounds up to the nearest multiple of mem_pagesize() */
#define PAGE_ALIGN(size) (((size) + (mem_pagesize()-1)) & ~(mem_pagesize()-1))

// Given a payload pointer, get the header or footer pointer
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))
#define FTRP(bp) ((char *)(bp)+GET_SIZE(HDRP(bp))-OVERHEAD)

// Given a payload pointer, get the next or previous payload pointer
#define NEXT(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))

// Given a pointer to a header, get or set its value
#define GET(p) (*(size_t *)(p))
#define PUT(p, val) (*(size_t *)(p) = (val))

// Combine a size and alloc bit
#define PACK(size, alloc) ((size) | (alloc))

// Given a header pointer, get the alloc or size 
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_SIZE(p) (GET(p) & ~0xF)


/* Set a block to allocated 
 * Update block headers/footers as needed 
 * Update free list if applicable 
 * Split block if applicable 
 */
static void set_allocated(void *b, size_t size);

/* Request more memory by calling mem_map 
 * Initialize the new chunk of memory as applicable 
 * Update free list if applicable 
 */
static void extend(size_t s);

/* Coalesce a free block if applicable 
 * Returns pointer to new coalesced block 
 */
static void* coalesce(void *bp);

static void add_free(void *to_add);
static void delete_free(free_list *free);
static void print_free(void);
static void print_page_byte(void *hp, size_t sz);
static void print_bp(void *bp);
static void print_freebp(void *bp);
static void check(void);

size_t page_multiples;
void *page_start;
void *heap_start;
void *heap_start1;
void *heap_start2;
void *first_bp;
free_list *free_start;
//size_t heaps[16];
//size_t heapsz[16];
int hi;
int a;
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
  a = 0;
  page_multiples = 1;
  hi = 0;
  free_start = NULL;
  extend(PAGESIZE);

  return 0;
}

/* 
 * mm_malloc - Allocate a block by using bytes from current_avail,
 *     grabbing a new page if necessary.
 */
void *mm_malloc(size_t size)
{
  printf("malloc #%d\n", a);
  a++;
  size = size < OVERHEAD ? OVERHEAD : size;
  int newsize = ALIGN(size) + OVERHEAD;
  
  free_list *free;
  
  for(free = free_start; free != NULL; free = free->next){
    if(GET_SIZE(HDRP(free)) >= newsize){
      set_allocated(free, newsize);
      return free;
    }
  }
  
  while(PAGE_ALIGN(newsize) >= PAGESIZE * page_multiples)
    page_multiples *= 2;

  //if we got this far, we need to extend the heap
  extend(PAGESIZE * page_multiples);

  set_allocated(free_start, newsize);
    
  return free_start;
}

void set_allocated(void *bp, size_t size) {
  size_t free_size = GET_SIZE(HDRP(bp));
  //free_size = free_size < OVERHEAD * sizeof(free_list 
  
  //need to split because we have extra room
  if(free_size >= 2 * OVERHEAD){
    delete_free(bp);

    PUT(HDRP(bp), PACK(size, 1));
    PUT(FTRP(bp), PACK(size, 1));
    bp = NEXT(bp);
    PUT(HDRP(bp), PACK(free_size - size, 0));
    PUT(FTRP(bp), PACK(free_size - size, 0));

    add_free(bp);
  }

  else{
    PUT(HDRP(bp), PACK(size, 1));
    PUT(HDRP(bp), PACK(size, 1));
  }
}

void add_free(void *to_add){

  free_list *free = (free_list *)to_add;
  
  if(free_start == NULL){
    free_start = free;
    free_start->next = NULL;
    free_start->prev = NULL;
  }
  else{
    free->next = free_start;
    free_start->prev = free;
    free_start = free;
  }
  
  return;
}

 void delete_free(free_list *free){

   //in the middle
   if(free->prev != NULL && free->next != NULL) {
       free->prev->next = free->next;
       free->next->prev = free->prev;
     }
   //last link in free list
   else if(free->prev != NULL) { 
     free->prev->next = NULL; 
   }
   //this must be free_start in nonempty list
   else if(free->next != NULL){
     free->next->prev = NULL;
     free_start = free->next;
   }
   //this must be free_start in an list of 1
   else{
     free_start = NULL;
   }
   free->next = NULL;
   free->prev = NULL;
 }

//Call mem_map to request a new page of size size.
//This will be at least 4096 size we PAGEALIGN size before calling extend.
//Note the initial page structure after extension:
//[empty] [16/1] [16/1] [size-32/0] [next] [prev]....[size-32/0][0,1]
//                     ^first_bp   ^free_list*       
void extend(size_t size){
  char* start;
  start = mem_map(size);

  PUT(start, 0);                                                             //padding so first bp is 16 byte aligned
  PUT(start + BYTE, PACK(OVERHEAD, 1));                                      //prologue header
  PUT(start + OVERHEAD, PACK(OVERHEAD, 1));                                  //prologue footer;
  PUT(start + size - sizeof(size_t), PACK(0, 1));                            //epilogue header
  
  first_bp = start + OVERHEAD;
  PUT(HDRP(NEXT(first_bp)), PACK(size - (OVERHEAD * 2), 0));
  PUT(FTRP(NEXT(first_bp)), PACK(size - (OVERHEAD * 2), 0));

  add_free(NEXT(first_bp));

  page_multiples *= 2;

  /* heaps[hi] = (size_t)start; */
  /* heapsz[hi] = size; */
  /* hi++; */
}


/*
 * Free the block associated with the supplied address, coalescing and releasing pages if necessary.
 */
void mm_free(void *ptr)
{

  size_t size = GET_SIZE(HDRP(ptr));

  PUT(HDRP(ptr), PACK(size, 0));
  PUT(FTRP(ptr), PACK(size, 0));
  
  ptr = coalesce(ptr);

  size = GET_SIZE(HDRP(ptr));
  size_t next_size = GET_SIZE(HDRP(NEXT(ptr)));
  size_t adj_size = size + OVERHEAD * 2;
  //When the free ptr size is a multiple of a page size (when including the total page overhead)
  //AND the next bp is the page footer, we need to unmap the page
  if(PAGE_ALIGN(adj_size) == adj_size && (next_size == 0)) {
      void* start = (void*) ((unsigned long) ptr - 2 * OVERHEAD);
      mem_unmap(start, adj_size);
      return;
    }

  add_free((free_list *)ptr);
}


/*Merge this block with other free blocks, making sure to be careful about maintaining the proper
free list state.  Note: this function is pretty much exactly the same as the book, except for the 
calls to remove old free list pointers*/
static void* coalesce(void *bp){

  size_t prev_alloc = GET_ALLOC(FTRP(PREV(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if(prev_alloc && next_alloc)
    return bp;
  else if(prev_alloc && !next_alloc){
    size += GET_SIZE(HDRP(NEXT(bp)));
    delete_free((free_list *)NEXT(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  }
  else if(!prev_alloc && next_alloc){
    size += GET_SIZE(HDRP(PREV(bp)));
    delete_free((free_list *)PREV(bp));
    PUT(HDRP(PREV(bp)), PACK(size, 0));
    PUT(FTRP(PREV(bp)), PACK(size, 0));
    bp = PREV(bp);
  }
  else{
    size += GET_SIZE(HDRP(PREV(bp))) + GET_SIZE(HDRP(NEXT(bp)));
    delete_free((free_list *)NEXT(bp));
    PUT(HDRP(PREV(bp)), PACK(size, 0));
    PUT(FTRP(PREV(bp)), PACK(size, 0));
    bp = PREV(bp);
  }
  return bp;
}



void shuffle(int *array, size_t n) {    
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int usec = tv.tv_usec;
  srand48(usec);


  if (n > 1) {
    size_t i;
    for (i = n - 1; i > 0; i--) {
      size_t j = (unsigned int) (drand48()*(i+1));
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}



//################
//      MAIN
//################
/* int main(){ */
/*   int j = 0;  */
/*   int i; */

/*   size_t addrs[16]; */
/*   mm_init(); */

/*   for(i = 0; i < 200; i++) */
/*     mm_malloc(4072); */

/*   for(i = 0; i < hi; i++) */
/*     print_page_byte((void*)heaps[i], heapsz[i]); */
/*   print_free(); */

/*   printf("heapsize: %ld\n", mem_heapsize()); */
/* } */





/*Debugging IO helpers*/
static void check(){
  void *l;
  for(l = first_bp; ; l = NEXT(l))
    {
      if(GET_SIZE(HDRP(l)) == 0){
	printf("[head(TERMINAL) sz: %ld, allc %ld]\n", GET_SIZE(HDRP(l)), GET_ALLOC(HDRP(l)));
	break;
      }
      print_bp(l);
           
      if((unsigned long) l % ALIGNMENT)
	printf("error, unaligned bp\n");
    }
}

static void print_bp(void *bp){
  printf("[head sz: %ld, allc: %ld]  ", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
  printf("[foot sz: %ld, allc: %ld]\n", GET_SIZE(FTRP(bp)), GET_ALLOC(FTRP(bp)));
}

static void print_freebp(void *bp){
  if(GET_ALLOC(HDRP(bp)) || GET_ALLOC(FTRP(bp)))
    printf("WARNING! ALLOCATED BIT SET IN FREE LIST BLOCK");
  printf("[this: %lx, prev: %lx, next: %lx] [size: %ld]\n", (unsigned long)bp % 10000, ((unsigned long) ((free_list *)bp)->prev) % 10000, ((unsigned long) ((free_list *)bp)->next) % 10000, GET_SIZE(HDRP(bp)));
}

static void print_free(){
  free_list *free;
  free_list *prev = NULL;
  for(free = free_start; free != NULL; free = free->next){
    if(free->prev != prev)
      printf("WARNING! mismatched prev ptrs in free list\n");
    print_freebp(free);
    prev = free;
  }
  printf("-------------------------------\n");
}

//Prints the given page byte by byte
static void print_page_byte(void* hs, size_t sz){
  char *start;
  int count = 0;
  long data = 0;
  for(start = hs; start < (char*)hs + sz; start += BYTE, count++){
    data = *((long*) start);
    if(data == 0 && start > (char*)hs){
      printf("All zeros until...\n");
      while(data == 0)
      {
	start += BYTE;
	count++;
	data = *((long*) start);
      }
  }
  if(data > 10000000)
    printf("addrs: %lx: contains pointer to: %lx\n", (unsigned long)(start) % 10000, data % 10000);
  else
    printf("addrs: %lx: contains value: %ld\n", (unsigned long)(start) % 10000, data);
}
  printf("total bytes: %d\n", count * BYTE);
  printf("-------------------------------\n");
}
/*End Debugging IO helpers*/
