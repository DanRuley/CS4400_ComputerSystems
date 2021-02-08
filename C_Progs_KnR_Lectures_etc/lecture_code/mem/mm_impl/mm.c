/* 
 * Simple, 32-bit and 64-bit clean allocator based on implicit free
 * lists, first-fit placement, and boundary tag coalescing, as described
 * in the CS:APP3e text. Blocks must be aligned to doubleword (8 byte) 
 * boundaries. Minimum block size is 16 bytes. 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mm.h"
#include "memlib.h"

#define NEXTFITx

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ //line:vm:mm:beginconst
#define DSIZE       8       /* Double word size (bytes) */
#define MINBLKSZ    16
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  //line:vm:mm:endconst 

#define MAX(x, y) ((x) > (y)? (x) : (y))  

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            //line:vm:mm:get
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    //line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)                   //line:vm:mm:getsize
#define GET_ALLOC(p) (GET(p) & 0x1)                    //line:vm:mm:getalloc

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      //line:vm:mm:hdrp
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp
/* $end mallocmacros */

static char *heap_listp = 0;

/* #ifdef NEXT_FIT */
/* static char *rover; */
/* #endif */

static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp);
static void checkheap();
static void checkblock(void *bp);





static void printblock(void *bp)
{
  if(GET_SIZE(bp) == 0)
    {
      printf("End of the line, buddy\n");
      return;
    }
  printf("[header | size : %ld, alloc: %c]\n[footer | size : %ld, alloc: %cd]\n", GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)), GET_SIZE(FTRP(bp)), GET_ALLOC(FTRP(bp)));
}

static void checkheap(){
  
  char *l = heap_listp;
  while(GET_SIZE(HDRP(l)) > 0)
    {
      printblock(l);
      checkblock(l);
      l = NEXT_BLKP(l);
    }
}

static void checkblock(void *bp){
  char *h = HDRP(bp);
  char *f = FTRP(bp);
  if((h + GET_SIZE(h) != f))
    printf("blocks unaligned!\n");
}







int mm_init(void) {
  //obviously a void * with value -1 is not a real address - error code
  if ((heap_listp = mem_sbrk(4*WSIZE)) == (void*)-1)
    return - 1;

  PUT(heap_listp, 0); //alignment padding
  PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); //prologue header
  PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); //prologue footer
  PUT(heap_listp + (3*WSIZE), PACK(0,1));      //epilogue header
  heap_listp += (2*WSIZE);

  if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
    return -1;

  return 0;
}









void *mm_malloc(size_t size)
{
  size_t asize;
  size_t extendsize;
  char *bp;

  if (size == 0)
    return NULL;

  if(size <= DSIZE)
    asize = 2*DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  if((bp = find_fit(asize)) != NULL)
    {
      place(bp, asize);
      return bp;
    }

  extendsize = MAX(asize, CHUNKSIZE);
  if((bp = extend_heap(extendsize/WSIZE)) == NULL)
    return NULL;

  place(bp, asize);

  checkheap();
  return bp;
}


void mm_free(void *bp)
{
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}


static void *find_fit(size_t asize){
  char *bp;

  for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp))
      if(!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize)
	return bp;
  
  return NULL;
}

static void place(void *bp, size_t asize){
  
  size_t old_sz = GET_SIZE(bp);

  PUT(HDRP(bp), PACK(asize, 1));
  PUT(FTRP(bp), PACK(asize, 1));

  if(old_sz - asize > MINBLKSZ){
    PUT(HDRP(NEXT_BLKP(bp)), PACK(old_sz - asize, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(old_sz - asize, 0));
  }
}

static void *coalesce(void *bp)
{
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if(prev_alloc && next_alloc)
    return bp;

  else if(!prev_alloc && next_alloc)
    {
      size_t newsize = GET_SIZE(FTRP(PREV_BLKP(bp)));
      PUT((HDRP(PREV_BLKP(bp))), PACK(newsize, 0));
      PUT((FTRP(PREV_BLKP(bp))), PACK(newsize, 0));
      bp = PREV_BLKP(bp);
    }

  else if(prev_alloc && !next_alloc)
    {
      size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
      PUT(HDRP(bp), PACK(size, 0));
      PUT(FTRP(bp), PACK(size, 0));
    }

  else
    {
      size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
      PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));
      bp = PREV_BLKP(bp);
    }

  return bp;
}


static void *extend_heap(size_t words)
{
  char *bp;
  size_t size;

  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;

  //bp will be starting address of new chunk
  if((long)(bp = mem_sbrk(size)) == -1)
    return NULL;

  //initialize free block header/footer and the epilogue header

  PUT(HDRP(bp), PACK(size, 0)); //free block header
  PUT(FTRP(bp), PACK(size, 0)); //free block footer
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); //new epilogue header

  //coalesce if prev block was free

  return coalesce(bp);
}


int main(){
  mem_init();

  if(mm_init() < 0){
    printf("Error initializing allocater\n");
    exit(-1);
  }
  else
    printf("Allocater initialized\n");

  void *mem = mm_malloc(12);
}
