#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "instruction.h"

#define OP_REG_MASK 0x000001f


int main()
{
  instruction_t blah;
  

  unsigned int testnum = 0x1040FFFE;
  
  blah.immediate = testnum & 0x0000ffff;
  printf("%d\n",blah.immediate);
int n = testnum & 0x0000ffff;
  printf("Immediate: %x\n", n);
  printf("opcode: %x\n", (testnum >> 27) & OP_REG_MASK);
  printf("reg 1: %x\n", (testnum >> 22) & OP_REG_MASK);
  printf("reg 2: %x\n", (testnum >> 17) & OP_REG_MASK);

  
  
  
  return 0;
}

/*
 * Decodes the array of raw instruction bytes into an array of instruction_t
 * Each raw instruction is encoded as a 4-byte unsigned int
 */
instruction_t* decode_instructions(unsigned int* bytes, unsigned int num_instructions)
{
  int i;
  int j;
  unsigned int tempnum;

  instruction_t* retval = (instruction_t*) malloc(num_instructions * sizeof(instruction_t));
  instruction_t temp;


  for(i = 0; i < num_instructions; i++){
    tempnum = 0;
    
    for(j = 0; j < 4; j++)
      tempnum |= bytes[i * 4 + j] << (8 * 3 - j); //shift first byte 24, second 16, third 8, fourth 0
      
    temp.immediate = tempnum & 0x0000ffff;
    temp.opcode = (tempnum >> 27) & OP_REG_MASK;
    temp.first_register = (tempnum >> 22) & OP_REG_MASK;
    temp.second_register = (tempnum >> 17) & OP_REG_MASK;
  }
  
  return retval;
}
