/*******************************************
 * Solutions for the CS:APP Performance Lab
 ********************************************/
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following student struct 
 */
student_t student = {
  "Dan Ruley",     /* Full name */
  "u0956834@utah.edu",  /* Email address */
};

/***************
 * COMPLEX KERNEL
 ***************/

/******************************************************
 * Your different versions of the complex kernel go here
 ******************************************************/

/* 
 * naive_complex - The naive baseline version of complex 
 */
char naive_complex_descr[] = "naive_complex: Naive baseline implementation";
void naive_complex(int dim, pixel *src, pixel *dest)
{
  int i, j;

  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
    {

      dest[RIDX(dim - j - 1, dim - i - 1, dim)].red = ((int)src[RIDX(i, j, dim)].red +
						      (int)src[RIDX(i, j, dim)].green +
						      (int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].green = ((int)src[RIDX(i, j, dim)].red +
							(int)src[RIDX(i, j, dim)].green +
							(int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].blue = ((int)src[RIDX(i, j, dim)].red +
						       (int)src[RIDX(i, j, dim)].green +
						       (int)src[RIDX(i, j, dim)].blue) / 3;

    }
}

char complex_block_descr[] = "complex: blocking version ~1.9 speedup";
static void complex_blocking(int dim, pixel *src, pixel *dest)
{
  int i, j, ii, jj, dim_sq;
    
  dim_sq = dim * dim;

  int W = dim == 1024 ? 32 : 16;

  for(i = 0; i < dim; i += W)
    for(j = 0; j < dim; j += W)
      for(ii = i; ii < i + W; ii++) {
  	int dimii = dim * ii;
  	for(jj = j; jj < j + W; jj++)
  	  {
  	    //Note:
  	    //RIDX(dim - ii - 1, dim - jj - 1, dim) = dim^2 - dim*ii - jj - 1
  	    int dst_offset = dim_sq - dimii - jj - 1;
	    
  	    int src_offset = RIDX(jj, ii, dim);

  	    unsigned short avg = (src[src_offset].red + src[src_offset].green + src[src_offset].blue) / 3;
	    
  	    dest[dst_offset].red = avg;
  	    dest[dst_offset].green = avg;
  	    dest[dst_offset].blue = avg;
  	  }
      }

}


/* 
 * complex - Your current working version of complex
 * IMPORTANT: This is the version you will be graded on
 */
char complex_descr[] = "complex: Current working version";
void complex(int dim, pixel *src, pixel *dest)
{
  complex_blocking(dim, src, dest);
}

/*********************************************************************
 * register_complex_functions - Register all of your different versions
 *     of the complex kernel with the driver by calling the
 *     add_complex_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_complex_functions() {
  add_complex_function(&complex, complex_descr);
  add_complex_function(&naive_complex, naive_complex_descr);
  add_complex_function(&complex_blocking, complex_block_descr);
}


/***************
 * MOTION KERNEL
 **************/

/***************************************************************
 * Various helper functions for the motion kernel
 * You may modify these or add new ones any way you like.
 **************************************************************/


/* 
 * weighted_combo - Returns new pixel value at (i,j) 
 */
static pixel weighted_combo(int dim, int i, int j, pixel *src) 
{
  int ii, jj;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;

  int num_neighbors = 0;
  for(ii=0; ii < 3; ii++){
    for(jj=0; jj < 3; jj++) 
      if ((i + ii < dim) && (j + jj < dim)) 
      {
	num_neighbors++;

	red += (int) src[RIDX(i+ii,j+jj,dim)].red;
	green += (int) src[RIDX(i+ii,j+jj,dim)].green;
	blue += (int) src[RIDX(i+ii,j+jj,dim)].blue;
      }

  }
  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);

  return current_pixel;
}


/*struct that keeps track of the cumulative sum.*/
struct int_px{
  unsigned int r;
  unsigned int g;
  unsigned int b;
};


/*Subtract the previous column*/
__attribute__((always_inline)) static void sub_last_col(int src_offset, char h, pixel *src, struct int_px *pp, int dim){
  
  char i;

  for(i = 0; i < h; i++){
 
    pp->r -= src[src_offset].red;
    pp->g -= src[src_offset].green;
    pp->b -= src[src_offset].blue;
    
    src_offset += dim;
  } 
}


/*Add the next column*/
__attribute__((always_inline)) static void add_next_col(int src_offset, char h, pixel *src, struct int_px *pp, int dim){

  char i;

  for(i = 0; i < h; i++){

    pp->r += src[src_offset].red;
    pp->g += src[src_offset].green;
    pp->b += src[src_offset].blue;
    
    src_offset += dim;
  }
}


/*Add up a box of pixels to start of the cumulative row sum*/
__attribute__((always_inline)) static void add_full_box(int i, int j, pixel *src, struct int_px *pp, int dim, char h){
  
  int src_offset;
  int ii, jj;

  for(ii = 0; ii < h; ii++) {
    src_offset = RIDX(i + ii, j, dim);
    for(jj = 0; jj < 3; jj++){
      pp->r += src[src_offset].red;
      pp->g += src[src_offset].green;
      pp->b += src[src_offset].blue;
      src_offset++;
    }
  }
}

/******************************************************
 * Your different versions of the motion kernel go here
 ******************************************************/


/*
 * naive_motion - The naive baseline version of motion 
 */
char naive_motion_descr[] = "naive_motion: Naive baseline implementation";
void naive_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;
    
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = weighted_combo(dim, i, j, src);
}

/*
 * Motion kernel using dynamic programming approach
 */
char dp_motion_descr[] = "Working version ~3.3 speedup, sort of a dp approach using cumulative sums";
static void dp_motion(int dim, pixel *src, pixel *dst){
  
  int i, j;
  struct int_px p, *pp;
  pp = &p;

  for(i = 0; i < dim - 2; i++) {
    
    p.r = p.g = p.b = 0;
    
    int dst_offset = RIDX(i, 0, dim);
   
    add_full_box(i, 0, src, pp, dim, 3);     

    dst[dst_offset].red = pp->r / 9;
    dst[dst_offset].green = pp->g / 9;
    dst[dst_offset].blue = pp->b / 9;


    for(j = 1; j < dim - 2; j++) {
      
      dst_offset++;
      sub_last_col(dst_offset - 1, 3, src, pp, dim);
      add_next_col(dst_offset + 2, 3, src, pp, dim);

      dst[dst_offset].red = pp->r / 9;
      dst[dst_offset].green = pp->g / 9;
      dst[dst_offset].blue = pp->b / 9;
    }

    //edges
    sub_last_col(dst_offset, 3, src, pp, dim);
    dst_offset++;

    dst[dst_offset].red = pp->r / 6;
    dst[dst_offset].green = pp->g / 6;
    dst[dst_offset].blue = pp->b / 6;

    sub_last_col(dst_offset, 3, src, pp, dim);
    dst_offset++;

    dst[dst_offset].red = pp->r / 3;
    dst[dst_offset].green = pp->g / 3;
    dst[dst_offset].blue = pp->b / 3;     

   
  }//end main i loop


  //Penultimate row
  int dst_offset = RIDX(i, 0, dim);

  p.r = p.g = p.b = 0;
  
  add_full_box(i, 0, src, pp, dim, 2);
  
  dst[dst_offset].red = pp->r / 6;
  dst[dst_offset].green = pp->g / 6;
  dst[dst_offset].blue = pp->b / 6;
  
  for(j = 1; j < dim - 2; j++) {
      
    dst_offset++;
    sub_last_col(dst_offset - 1, 2, src, pp, dim);
    add_next_col(dst_offset + 2, 2, src, pp, dim);

    dst[dst_offset].red = pp->r / 6;
    dst[dst_offset].green = pp->g / 6;
    dst[dst_offset].blue = pp->b / 6;
  }

  //edges
  sub_last_col(dst_offset, 2, src, pp, dim);
  dst_offset++;

  dst[dst_offset].red = pp->r >> 2;
  dst[dst_offset].green = pp->g >> 2;
  dst[dst_offset].blue = pp->b >> 2;

      
  sub_last_col(dst_offset, 2, src, pp, dim);
  dst_offset++;

  dst[dst_offset].red = pp->r >> 1;
  dst[dst_offset].green = pp->g >> 1;
  dst[dst_offset].blue = pp->b >> 1;
    


  //Final row
  i++;

  dst_offset = RIDX(i, 0, dim);

  p.r = p.g = p.b = 0;

  add_full_box(i, 0, src, pp, dim, 1);
  
  dst[dst_offset].red = pp->r / 3;
  dst[dst_offset].green = pp->g / 3;
  dst[dst_offset].blue = pp->b / 3;
  

  for(j = 1; j < dim - 2; j++) {
    
    dst_offset++;
    sub_last_col(dst_offset - 1, 1, src, pp, dim);
    add_next_col(dst_offset + 2, 1, src, pp, dim);

    dst[dst_offset].red = pp->r / 3;
    dst[dst_offset].green = pp->g / 3;
    dst[dst_offset].blue = pp->b / 3;
  }
  
  //edges
  sub_last_col(dst_offset, 1, src, pp, dim);
  dst_offset++;

  dst[dst_offset].red = pp->r >> 1;
  dst[dst_offset].green = pp->g >> 1;
  dst[dst_offset].blue = pp->b >> 1;
      
  sub_last_col(dst_offset, 1, src, pp, dim);
  dst_offset++;

  dst[dst_offset].red = pp->r;
  dst[dst_offset].green = pp->g;
  dst[dst_offset].blue = pp->b;
}


/*
 * motion - Your current working version of motion. 
 * IMPORTANT: This is the version you will be graded on
 */
char motion_descr[] = "motion: Current working version";
void motion(int dim, pixel *src, pixel *dst) 
{
  dp_motion(dim, src, dst);
}

/********************************************************************* 
 * register_motion_functions - Register all of your different versions
 *     of the motion kernel with the driver by calling the
 *     add_motion_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_motion_functions() {
  add_motion_function(&motion, motion_descr);
  add_motion_function(&naive_motion, naive_motion_descr);
  add_motion_function(&dp_motion, dp_motion_descr);
}
