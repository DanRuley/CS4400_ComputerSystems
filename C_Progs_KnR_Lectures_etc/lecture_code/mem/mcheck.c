/*
 * Based on:
 *
 *  mdriver.c - CS:APP Malloc Lab Driver
 * 
 * Uses a collection of trace files to tests a malloc/free
 * implementation.
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>

int mm_init();
void *mm_malloc(size_t);
void mm_free(void *);

/**********************
 * Constants and macros
 **********************/

/* 
 * Alignment requirement in bytes
 */
#define ALIGNMENT 16

/* Returns true if p is ALIGNMENT-byte aligned */
#define IS_ALIGNED(p)  ((((uintptr_t)(p)) % ALIGNMENT) == 0)

/* Misc */
#define MAXLINE     1024 /* max string size */
#define HDRLINES       4 /* number of header lines in a trace file */
#define LINENUM(i) (i+5) /* cnvt trace request nums to linenums (origin 1) */

/****************************** 
 * The key compound data types 
 *****************************/

/* Records the extent of each block's payload */
typedef struct range_t {
    char *lo;              /* low payload address */
    char *hi;              /* high payload address */
    struct range_t *next;
} range_t;

range_t ranges[1<<14];
range_t *free_ranges;

/* Characterizes a single trace operation (allocator request) */
typedef struct {
    enum {ALLOC, FREE, REALLOC} type; /* type of request */
    int index;                        /* index for free() to use later */
    int size;                         /* byte size of alloc/realloc request */
} traceop_t;

/* Holds the information for one trace file*/
typedef struct {
    int sugg_heapsize;   /* suggested heap size (unused) */
    int num_ids;         /* number of alloc/realloc ids */
    int num_ops;         /* number of distinct requests */
    int weight;          /* weight for this trace (unused) */
    traceop_t *ops;      /* array of requests */
    char **blocks;       /* array of ptrs returned by malloc/realloc... */
    size_t *block_sizes; /* ... and a corresponding array of payload sizes */
} trace_t; 

/********************
 * Global variables
 *******************/
int verbose = 0;        /* global flag for verbose output */
static int errors = 0;  /* number of errs found when running student malloc */
char msg[MAXLINE];      /* for whenever we need to compose an error message */

/* Directory where default tracefiles are found */
static char tracedir[MAXLINE] = "traces";

/* The filenames of the default tracefiles */
static char *default_tracefiles[] = {  
  "amptjp-bal.rep",
  "cccp-bal.rep",
  "cp-decl-bal.rep",
  "expr-bal.rep",
  "coalescing-bal.rep",
  "random-bal.rep",
  "random2-bal.rep",
  "binary-bal.rep",
  "binary2-bal.rep",
  NULL
};


/********************* 
 * Function prototypes 
 *********************/

/* these functions manipulate range lists */
static void reset_ranges();
static int add_range(range_t **ranges, char *lo, int size, 
		     int tracenum, int opnum);
static void remove_range(range_t **ranges, char *lo);

/* These functions read, allocate, and free storage for traces */
static void read_trace(trace_t *t, char *tracedir, char *filename);

/* Routines for evaluating correctnes, space utilization, and speed 
   of the student's malloc package in mm.c */
static int eval_mm_valid(trace_t *trace, int tracenum, range_t **ranges);

/* Various helper routines */
static void usage(void);
static void unix_error(char *msg);
static void malloc_error(int tracenum, int opnum, char *msg);
static void app_error(char *msg);

/**************
 * Main routine
 **************/
int main(int argc, char **argv)
{
    int i;
    char c;
    char **tracefiles = NULL;  /* null-terminated array of trace file names */
    int num_tracefiles = 0;    /* the number of traces in that array */
    trace_t *traces;
    range_t *ranges = NULL;    /* keeps track of block extents for one trace */
    
    /* 
     * Read and interpret the command line arguments 
     */
    while ((c = getopt(argc, argv, "f:t:hVa")) != EOF) {
        switch (c) {
        case 'f': /* Use one specific trace file only (relative to curr dir) */
            num_tracefiles = 1;
            if ((tracefiles = realloc(tracefiles, 2*sizeof(char *))) == NULL)
		unix_error("ERROR: realloc failed in main");
	    strcpy(tracedir, "./"); 
            tracefiles[0] = strdup(optarg);
            tracefiles[1] = NULL;
            break;
	case 't': /* Directory where the traces are located */
	    if (num_tracefiles == 1) /* ignore if -f already encountered */
		break;
	    strcpy(tracedir, optarg);
	    if (tracedir[strlen(tracedir)-1] != '/') 
		strcat(tracedir, "/"); /* path always ends with "/" */
	    break;
        case 'V': /* Be verbose */
            verbose = 2;
            break;
        case 'h': /* Print this message */
	    usage();
            exit(0);
        default:
	    usage();
            exit(1);
        }
    }
	
    /* 
     * If no -f command line arg, then use the entire set of tracefiles 
     * defined in default_traces[]
     */
    if (tracefiles == NULL) {
        tracefiles = default_tracefiles;
        num_tracefiles = sizeof(default_tracefiles) / sizeof(char *) - 1;
	printf("Using default tracefiles in %s\n", tracedir);
    }

    traces = malloc(num_tracefiles * sizeof(trace_t));
    for (i=0; i < num_tracefiles; i++) {
      read_trace(traces + i, tracedir, tracefiles[i]);
    }

    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
    /* No more calls to malloc() after this point! */
    /*  since mm_alloc() & co can use sbrk()       */
    /* No more calls to printf(), since it uses    */
    /*  malloc()                                   */
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

    /*
     * Always run and evaluate the student's mm package
     */
    if (verbose > 1)
      write(1, "\nTesting mm malloc\n", 19);

    /* Evaluate mm malloc */
    for (i=0; i < num_tracefiles; i++) {
      if (verbose > 1) {
        write(1, "Checking mm_malloc on ", 22);
	write(1, tracefiles[i], strlen(tracefiles[i]));
	write(1, ", ", 2);
      }
      if (eval_mm_valid(&traces[i], i, &ranges)) {
        if (verbose > 1)
          write(1, "ok\n", 3);
      } else
        printf("failed for %s\n", tracefiles[i]);
    }

    return 0;
}


/*****************************************************************
 * The following routines manipulate the range list, which keeps 
 * track of the extent of every allocated block payload. We use the 
 * range list to detect any overlapping allocated blocks.
 ****************************************************************/

static void reset_ranges() {
  int i;
  free_ranges = NULL;
  for (i = 0; i < sizeof(ranges) / sizeof(ranges[0]); i++) {
    ranges[i].next = free_ranges;
    free_ranges = &ranges[i];
  }
}

/*
 * add_range - As directed by request opnum in trace tracenum,
 *     we've just called the student's mm_malloc to allocate a block of 
 *     size bytes at addr lo. After checking the block for correctness,
 *     we create a range struct for this block and add it to the range list. 
 */
static int add_range(range_t **ranges, char *lo, int size, 
		     int tracenum, int opnum)
{
    char *hi = lo + size - 1;
    range_t *p;
    char msg[MAXLINE];

    assert(size > 0);

    /* Payload addresses must be ALIGNMENT-byte aligned */
    if (!IS_ALIGNED(lo)) {
	sprintf(msg, "Payload address (%p) not aligned to %d bytes", 
		lo, ALIGNMENT);
        malloc_error(tracenum, opnum, msg);
        return 0;
    }

    /* The payload must not overlap any other payloads */
    for (p = *ranges;  p != NULL;  p = p->next) {
        if ((lo >= p->lo && lo <= p-> hi) ||
            (hi >= p->lo && hi <= p->hi)) {
	    sprintf(msg, "Payload (%p:%p) overlaps another payload (%p:%p)\n",
		    lo, hi, p->lo, p->hi);
	    malloc_error(tracenum, opnum, msg);
	    return 0;
        }
    }

    /* 
     * Everything looks OK, so remember the extent of this block 
     * by creating a range struct and adding it the range list.
     */
    p = free_ranges;
    if (p == NULL)
	unix_error("out of space in add_range");
    free_ranges = free_ranges->next;
    p->next = *ranges;
    p->lo = lo;
    p->hi = hi;
    *ranges = p;
    return 1;
}

/* 
 * remove_range - Free the range record of block whose payload starts at lo 
 */
static void remove_range(range_t **ranges, char *lo)
{
    range_t *p;
    range_t **prevpp = ranges;

    for (p = *ranges;  p != NULL; p = p->next) {
        if (p->lo == lo) {
	    *prevpp = p->next;
            p->next = free_ranges;
            free_ranges = p;
            break;
        }
        prevpp = &(p->next);
    }
}

/**********************************************
 * The following routines manipulate tracefiles
 *********************************************/

/*
 * read_trace - read a trace file and store it in memory
 */
static void read_trace(trace_t *trace, char *tracedir, char *filename)
{
    FILE *tracefile;
    char type[MAXLINE];
    char path[MAXLINE];
    unsigned index, size;
    unsigned max_index = 0;
    unsigned op_index;

    if (verbose > 1)
	printf("Reading tracefile: %s\n", filename);

    /* Read the trace file header */
    strcpy(path, tracedir);
    strcat(path, filename);
    if ((tracefile = fopen(path, "r")) == NULL) {
	sprintf(msg, "Could not open %s in read_trace", path);
	unix_error(msg);
    }
    fscanf(tracefile, "%d", &(trace->sugg_heapsize)); /* not used */
    fscanf(tracefile, "%d", &(trace->num_ids));     
    fscanf(tracefile, "%d", &(trace->num_ops));     
    fscanf(tracefile, "%d", &(trace->weight));        /* not used */

    /* We'll store each request line in the trace in this array */
    if ((trace->ops = 
	 (traceop_t *)malloc(trace->num_ops * sizeof(traceop_t))) == NULL)
	unix_error("malloc 2 failed in read_trace");

    /* We'll keep an array of pointers to the allocated blocks here... */
    if ((trace->blocks = 
	 (char **)malloc(trace->num_ids * sizeof(char *))) == NULL)
	unix_error("malloc 3 failed in read_trace");

    /* ... along with the corresponding byte sizes of each block */
    if ((trace->block_sizes = 
	 (size_t *)malloc(trace->num_ids * sizeof(size_t))) == NULL)
	unix_error("malloc 4 failed in read_trace");
    
    /* read every request line in the trace file */
    index = 0;
    op_index = 0;
    while (fscanf(tracefile, "%s", type) != EOF) {
	switch(type[0]) {
	case 'a':
	    fscanf(tracefile, "%u %u", &index, &size);
	    trace->ops[op_index].type = ALLOC;
	    trace->ops[op_index].index = index;
	    trace->ops[op_index].size = size;
	    max_index = (index > max_index) ? index : max_index;
	    break;
	case 'r':
	    fscanf(tracefile, "%u %u", &index, &size);
	    trace->ops[op_index].type = REALLOC;
	    trace->ops[op_index].index = index;
	    trace->ops[op_index].size = size;
	    max_index = (index > max_index) ? index : max_index;
	    break;
	case 'f':
	    fscanf(tracefile, "%ud", &index);
	    trace->ops[op_index].type = FREE;
	    trace->ops[op_index].index = index;
	    break;
	default:
	    printf("Bogus type character (%c) in tracefile %s\n", 
		   type[0], path);
	    exit(1);
	}
	op_index++;
	
    }
    fclose(tracefile);
    assert(trace->num_ops == op_index);
}

/**********************************************************************
 * The following functions evaluate the correctness, space utilization,
 * and throughput of the libc and mm malloc packages.
 **********************************************************************/

/*
 * eval_mm_valid - Check the mm malloc package for correctness
 */
static int eval_mm_valid(trace_t *trace, int tracenum, range_t **ranges) 
{
    int i;
    int index;
    int size;
    char *newp;
    char *oldp;
    char *p;

    reset_ranges();
    *ranges = NULL;
    
    /* Call the mm package's init function */
    if (mm_init() < 0) {
	malloc_error(tracenum, 0, "mm_init failed.");
	return 0;
    }

    /* Interpret each operation in the trace in order */
    for (i = 0;  i < trace->num_ops;  i++) {
	index = trace->ops[i].index;
	size = trace->ops[i].size;

        switch (trace->ops[i].type) {

        case ALLOC: /* mm_malloc */

	    /* Call the student's malloc */
	    if ((p = mm_malloc(size)) == NULL) {
		malloc_error(tracenum, i, "mm_malloc failed.");
		return 0;
	    }
	    
	    /* 
	     * Test the range of the new block for correctness and add it 
	     * to the range list if OK. The block must be  be aligned properly,
	     * and must not overlap any currently allocated block. 
	     */ 
	    if (add_range(ranges, p, size, tracenum, i) == 0)
		return 0;
	    
	    /* ADDED: cgw
	     * fill range with low byte of index.  This will be used later
	     * if we realloc the block and wish to make sure that the old
	     * data was copied to the new block
	     */
	    memset(p, index & 0xFF, size);

	    /* Remember region */
	    trace->blocks[index] = p;
	    trace->block_sizes[index] = size;
	    break;

        case REALLOC: /* mm_malloc + mm_free */
	    
	    /* Call the student's realloc */
	    oldp = trace->blocks[index];
	    if ((newp = mm_malloc(size)) == NULL) {
		malloc_error(tracenum, i, "mm_malloc failed.");
		return 0;
	    }

	    /* Remove the old region from the range list */
	    remove_range(ranges, oldp);
	    
	    /* Check new block for correctness and add it to range list */
	    if (add_range(ranges, newp, size, tracenum, i) == 0)
		return 0;

	    memset(newp, index & 0xFF, size);

            mm_free(oldp);

	    /* Remember region */
	    trace->blocks[index] = newp;
	    trace->block_sizes[index] = size;
	    break;

        case FREE: /* mm_free */
	    
	    /* Remove region from list and call student's free function */
	    p = trace->blocks[index];
	    remove_range(ranges, p);
	    mm_free(p);
	    break;

	default:
	    app_error("Nonexistent request type in eval_mm_valid");
        }

    }

    /* As far as we know, this is a valid malloc package */
    return 1;
}

/* 
 * app_error - Report an arbitrary application error
 */
void app_error(char *msg) 
{
    printf("%s\n", msg);
    exit(1);
}

/* 
 * unix_error - Report a Unix-style error
 */
void unix_error(char *msg) 
{
    printf("%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * malloc_error - Report an error returned by the mm_malloc package
 */
void malloc_error(int tracenum, int opnum, char *msg)
{
    errors++;
    printf("ERROR [trace %d, line %d]: %s\n", tracenum, LINENUM(opnum), msg);
}

/* 
 * usage - Explain the command line arguments
 */
static void usage(void) 
{
    fprintf(stderr, "Usage: mdriver [-hvVal] [-f <file>] [-t <dir>]\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "\t-f <file>  Use <file> as the trace file.\n");
    fprintf(stderr, "\t-g         Generate summary info for autograder.\n");
    fprintf(stderr, "\t-h         Print this message.\n");
    fprintf(stderr, "\t-l         Run libc malloc as well.\n");
    fprintf(stderr, "\t-t <dir>   Directory to find default traces.\n");
    fprintf(stderr, "\t-v         Print per-trace performance breakdowns.\n");
    fprintf(stderr, "\t-V         Print additional debug info.\n");
}
