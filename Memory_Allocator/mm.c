/*
 * Mallocater 9001 - Dan Ruley u0956834
 * 
 * This allocater uses an explicit free list to manage the free blocks.  The explicit list is a kind of
 * linked list stack with O(1) add and removes.  These links "thread" the pages together so there is
 * no need to waste space on page pointer overhead.
 *
 * Memory is allocated using first-fit placing.  Pages are requested and then released when they contained no
 * allocated blocks with the exception of a "home page" - a small "cache" that is allocated and never
 * released throughout the allocators lifetime.  
 *
 * This may not be the best practice, but it ended up massively increasing performance because there are a 
 * number of trace files that mercilessly thrash the page if you ALWAYS deallocate empty pages.  The multiples 
 * for page extension and the initial home page were experimentally determined.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

//Free list struct
typedef struct free_list{
  struct free_list *prev;
  struct free_list *next;
}free_list;

typedef size_t block_header;
typedef size_t block_footer;

//Constants
#define PAGESIZE 4096
#define OVERHEAD (sizeof(block_header)+sizeof(block_footer))
#define PAGE_OVERHEAD (sizeof(block_header) * 4)
#define ALIGNMENT 16
#define BYTE 8

//Experimentially determined page increment multiple - I have no idea why this works
#define MULTIPLE 51

//Experimentially determined multiple for my "home_pg" - a kind of small page cache
//I never release - this avoids the constant page thrashing caused by some of the traces
//Again, absolutely no idea why this value is better than others ¯\_(ツ)_/¯
#define HOMEPG_MULTIPLE 5

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
static void *set_allocated(void *b, size_t size);

/* Request more memory by calling mem_map 
 * Initialize the new chunk of memory as applicable 
 * Update free list if applicable 
 */
static void *extend(size_t s);

/* Coalesce a free block if applicable 
 * Returns pointer to new coalesced block 
 */
static void* coalesce(void *bp);

/*push a free block on the free list*/
static void push_free(void *to_add);

/*delete a free block from the free list*/
static void pop_free(void *to_del);

static void free_homepg();

//Debugging IO helpers - poorly named for fast gdb usage
static void pf(void);
static void phb(void *hp, size_t sz);
static void print_bp(void *bp);
static void print_freebp(void *bp);
static void chk(void *bp);

void *home_pg;
free_list *free_start;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
  home_pg = NULL;
  free_start = NULL;
  extend(PAGESIZE * 5);
  return 0;
}

/* 
 * mm_malloc - Allocate a block by using bytes from current_avail,
 *     grabbing a new page if necessary.
 */
void *mm_malloc(size_t size)
{
  //enforces a 32byte minimum size so there is always room for free ptrs
  size = size < OVERHEAD ? OVERHEAD : size;
  size_t newsize = ALIGN(size + OVERHEAD);
  
  free_list *free;
  
  for(free = free_start; free != NULL; free = free->next)
    if(GET_SIZE(HDRP(free)) >= newsize)
      return set_allocated(free, newsize);

  void *add;
  //if we got this far, we need to extend the heap
  if(PAGE_ALIGN(newsize) > PAGESIZE * MULTIPLE)
    add = extend(PAGE_ALIGN(newsize) * MULTIPLE);
  else
    add = extend(PAGESIZE * MULTIPLE);


  return set_allocated(add, newsize);
}

/*
 * Free the block associated with the supplied address, coalescing and releasing pages if necessary.
 */
void mm_free(void *bp)
{
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  
  bp = coalesce(bp);

  size = GET_SIZE(HDRP(bp));
  
  //We know we can free the page if the size of free's prev is 16 and next is 0 b/c
  //that page consists of only padding and one free block
  if(GET_SIZE(HDRP(PREV(bp))) == OVERHEAD && GET_SIZE(HDRP(NEXT(bp))) == 0) {
    void* start = (void*) ((unsigned long) bp - 2 * OVERHEAD);        

    if (start == home_pg)
      return;

    pop_free(bp);

    mem_unmap(start, size + PAGE_OVERHEAD);
  }
}


//Call mem_map to request a new page of size size.
//This will be at least 4096 size we PAGEALIGN size before calling extend.
//Note the initial page structure after extension:
//[empty] [16/1] [16/1] [size-32/0] [next] [prev]....[size-32/0][0,1]
//                     ^first_bp   ^free_list*       
static void *extend(size_t size){
  char* start;
  start = mem_map(size);

  PUT(start, 0);                                                             //padding so first bp is 16 byte aligned
  PUT(start + BYTE, PACK(OVERHEAD, 1));                                      //prologue header
  PUT(start + OVERHEAD, PACK(OVERHEAD, 1));                                  //prologue footer
  PUT(start + size - sizeof(size_t), PACK(0, 1));                            //epilogue header
  
  PUT(start + OVERHEAD + BYTE, PACK(size - (OVERHEAD * 2), 0));
  PUT(FTRP(start + OVERHEAD * 2), PACK(size - (OVERHEAD * 2), 0));

  push_free(start + OVERHEAD * 2);

  home_pg = (home_pg == NULL) ? start : home_pg;

  return (void*)(start + OVERHEAD * 2);
}


/*Set the block as allocated, splitting as necessary*/
static void *set_allocated(void *bp, size_t alloc_size) {

  void *ret;
  size_t free_size = GET_SIZE(HDRP(bp));
   
  //need to split because we have extra room
  if(free_size - alloc_size >= 2 * OVERHEAD){
    PUT(HDRP(bp), PACK(alloc_size, 1));
    PUT(FTRP(bp), PACK(alloc_size, 1));
    ret = bp;
    void *old = bp;
    pop_free(old);

    bp = NEXT(bp);
    PUT(HDRP(bp), PACK(free_size - alloc_size, 0));
    PUT(FTRP(bp), PACK(free_size - alloc_size, 0));
    push_free(bp);
  }

  else{   
    ret = bp;
    PUT(HDRP(bp), PACK(free_size, 1));
    PUT(FTRP(bp), PACK(free_size, 1));
    pop_free(bp);
  }

  return ret;
}

/*Merge this block with other free blocks, making sure to be careful about maintaining the proper
free list state.  Note: this function is pretty much exactly the same as the book, except for the 
calls to remove old free list pointers*/
static void* coalesce(void *bp){

  size_t prev_alloc = GET_ALLOC(FTRP(PREV(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if(prev_alloc && next_alloc){
    push_free(bp);
    return bp;
  }
  else if(prev_alloc && !next_alloc){
    size += GET_SIZE(HDRP(NEXT(bp)));
    pop_free(NEXT(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    push_free(bp);
  }
  else if(!prev_alloc && next_alloc){
    size += GET_SIZE(HDRP(PREV(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV(bp)), PACK(size, 0));
    bp = PREV(bp);
  }
  else{
    size += GET_SIZE(HDRP(PREV(bp))) + GET_SIZE(HDRP(NEXT(bp)));
    pop_free(NEXT(bp));
    PUT(HDRP(PREV(bp)), PACK(size, 0));
    PUT(FTRP(PREV(bp)), PACK(size, 0));
    bp = PREV(bp);
  }

  return bp;
}

/*Add the free_list* to the free list*/
static void push_free(void *to_add){

  free_list *free = (free_list *)to_add;
  
  if(free_start == NULL){
    free_start = free;
    free_start->next = NULL;
    free_start->prev = NULL;
  }
  else{
    //add on the "stack"
    free->prev = NULL;
    free->next = free_start;
    free_start->prev = free;
    free_start = free;
  }
}

/*Delete the free_list * from the free list*/
 static void pop_free(void *to_del){
   free_list *free = (free_list *)to_del;

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
     free_start = free->next;
     free_start->prev = NULL;
   }
   //this must be free_start in an list of 1
   else{
     free_start->prev = NULL;
     free_start->next = NULL;
     free_start = NULL;
   }
 }







  








/*Debugging IO helpers*/
static void chk(void *bp){
  void *l;
  for(l = bp + 16; ; l = NEXT(l))
    {
      if(GET_ALLOC(HDRP(l)) && GET_SIZE(HDRP(l)) == 0){
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
  if(GET_ALLOC(HDRP(bp)) || GET_ALLOC(FTRP(bp))){
    printf("WARNING! Allocated bit set in the free list!");
  }
  printf("[this: %lx, prev: %lx, next: %lx] [size: %ld]\n", (unsigned long)bp % 10000, ((unsigned long) ((free_list *)bp)->prev) % 10000, ((unsigned long) ((free_list *)bp)->next) % 10000, GET_SIZE(HDRP(bp)));
}

static void pf(){
  free_list *free;
  free_list *prev = NULL;
  for(free = free_start; free != NULL; free = free->next){
    if(free->prev != prev){
      printf("WARNING! mismatched prev ptrs in free list\n");
      exit(0);
    }
    print_freebp(free);
    prev = free;
  }
  printf("-------------------------------\n");
}

//Prints the given page byte by byte
static void phb(void* hs, size_t sz){
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

static void shuffle(size_t arr[], size_t n){
  size_t random[100];
  time_t t;
  srand((unsigned) time(&t));
  size_t temp;
  int i;
  for(i = 0; i < n; i++)
    {
      size_t j = rand() % n;
      temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
}
/*End Debugging IO helpers*/


//################
//      MAIN
/* //################ */
/* int main(){ */
  
/*   int j = 0; */
/*   int i; */
/*   time_t t; */
/*   size_t random[1000]; */
/*   srand((unsigned) time(&t)); */

/*   mm_init(); */
  
/*   for(i = 0; i < 1000; i++) */
/*     random[i] = (size_t)mm_malloc(rand() % 3000); */

/*   /\* for(i = 0; i < hi; i++) *\/ */
/*   /\*   { *\/ */
/*   /\*     if(hs[i] != NULL){ *\/ */
/*   /\* 	printf("checking heap %d with size: %ld\n", i, hsz[i]); *\/ */
/*   /\* 	chk(hs[i]); *\/ */
/*   /\*     } *\/ */
/*   /\*   } *\/ */
  
/*   shuffle(random, 1000); */
  
/*   for(i = 0; i < 1000; i++) */
/*     mm_free((void*) random[i]); */
  
/*   printf("\n"); */
/*   printf("heapsize: %ld\n", mem_heapsize()); */
/* } */
