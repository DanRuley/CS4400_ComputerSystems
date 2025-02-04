/*
 * Author: Daniel Kopta
 * Updated by: Erin Parker
 * CS 4400, University of Utah
 *
 * Simulator handout
 * A simple x86-like processor simulator.
 * Read in a binary file that encodes instructions to execute.
 * Simulate a processor by executing instructions one at a time and appropriately 
 * updating register and memory contents.
 *
 * Some code and pseudo code has been provided as a starting point.
 *
 * Completed by: Dan Ruley, u0956834
 * September, 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "instruction.h"

// 17 registers
#define NUM_REGS 17

// 1024-byte stack
#define STACK_SIZE 1024

//This mask will work for op, reg1, reg2 as long as the number has been shifted appropriately.
#define OP_REG_MASK  0x0000001f 

//For extracting bits 0-15 for the immediate value.
#define IMM_MASK 0x0000FFFF

//Used to or with immediates for sign extension
#define SIGN_EXT_MASK 0xFFFF0000


//Pointers to the specially named registers %esp and %eflags.
int* esp;
unsigned int* eflags;

// Forward declarations for helper functions
unsigned int get_file_size(int file_descriptor);
unsigned int* load_file(int file_descriptor, unsigned int size);
instruction_t* decode_instructions(unsigned int* bytes, unsigned int num_instructions);
unsigned int execute_instruction(unsigned int program_counter, instruction_t* instructions, 
				 int* registers, unsigned char* memory);
void print_instructions(instruction_t* instructions, unsigned int num_instructions);
void error_exit(const char* message);

/*Main entry point for the program.*/
int main(int argc, char** argv)
{
  // Make sure we have enough arguments
  if(argc < 2)
    error_exit("must provide an argument specifying a binary file to execute");

  // Open the binary file
  int file_descriptor = open(argv[1], O_RDONLY);
  if (file_descriptor == -1) 
    error_exit("unable to open input file");

  // Get the size of the file
  unsigned int file_size = get_file_size(file_descriptor);
  // Make sure the file size is a multiple of 4 bytes
  // since machine code instructions are 4 bytes each
  if(file_size % 4 != 0)
    error_exit("invalid input file");

  // Load the file into memory
  // We use an unsigned int array to represent the raw bytes
  // We could use any 4-byte integer type
  unsigned int* instruction_bytes = load_file(file_descriptor, file_size);
  close(file_descriptor);

  unsigned int num_instructions = file_size / 4;
  int i;

  // Allocate and decode instructions
  instruction_t* instructions = decode_instructions(instruction_bytes, num_instructions);

  //print_instructions(instructions, num_instructions);

  // Allocate and initialize registers
  int* registers = (int*)malloc(sizeof(int) * NUM_REGS);

  //Set all registers to initial value of 0
  for(i = 0; i < NUM_REGS; i++)
    registers[i] = 0;
  
  esp = &registers[6];
  *esp = STACK_SIZE;
  eflags = &registers[16];

  // Stack memory is byte-addressed, so it must be a 1-byte type
  // Allocate the stack memory - Since a char is 1 byte, we allocate sizeof(char) * STACK_SIZE.
  unsigned char* memory = (unsigned char*) malloc(sizeof(char) * STACK_SIZE);
  for(i = 0; i < STACK_SIZE; i++)
    memory[i] = 0;

  // Run the simulation
  unsigned int program_counter = 0;

  // program_counter is a byte address, so we must multiply num_instructions by 4 
  // to get the address past the last instruction
  while(program_counter != num_instructions * 4)
  {
    program_counter = execute_instruction(program_counter, instructions, registers, memory);
  }
  
  
  return 0;
}


/*
 * Decodes the array of raw instruction bytes into an array of instruction_t
 * Each raw instruction is encoded as a 4-byte unsigned int
 *
 *                                Instruction format
 * Bits:           [31...27]   [26...22]   [21...17]   [16]   [15...0]
 *Interpretation:  [opcode ]   [reg1]      [reg2]      [x]    [immediate] 
*/
instruction_t* decode_instructions(unsigned int* bytes, unsigned int num_instructions)
{
  int i;
  unsigned int tempnum;
  instruction_t temp;

  instruction_t* decoded_instr = (instruction_t*) malloc(num_instructions * sizeof(instruction_t));
  
  for(i = 0; i < num_instructions; i++){    
    tempnum = bytes[i];
  
    //Extract appropriate bits and set the proper values in the temp instruction.
    temp.immediate = tempnum & IMM_MASK;
    temp.opcode = (tempnum >> 27) & OP_REG_MASK;
    temp.first_register = (tempnum >> 22) & OP_REG_MASK;
    temp.second_register = (tempnum >> 17) & OP_REG_MASK;

    decoded_instr[i] = temp;
  }

  return decoded_instr;
}


/*
 * Executes a single instruction and returns the next program counter
 */
unsigned int execute_instruction(unsigned int program_counter, instruction_t* instructions, int* registers, unsigned char* memory)
{
  // program_counter is a byte address, but instructions are 4 bytes each
  // divide by 4 to get the index into the instructions array
  instruction_t instr = instructions[program_counter / 4];

  //temporary variables used for computations, mainly so I didn't have to keep typing registers[instruction.first....
  unsigned int reg_num;
  int reg1, reg2;

  switch(instr.opcode)
    {

      /*Mathematical Instructions:*/
    
    case subl:  //opcode: 0
      
      registers[instr.first_register] = registers[instr.first_register] - instr.immediate;
      break;


    case addl_reg_reg: //opcode: 1
      
      registers[instr.second_register] = registers[instr.first_register] + registers[instr.second_register];
      break;


    case addl_imm_reg: //opcode: 2
      
      registers[instr.first_register] =  registers[instr.first_register] + instr.immediate;
      break;

  
    case imull: //opcode: 3
      
      registers[instr.second_register] = registers[instr.first_register] * registers[instr.second_register];
      break; 


    case shrl: //opcode: 4
      
      registers[instr.first_register] =  ((unsigned int)registers[instr.first_register]) >> 1;
      break;



      /*Move instructions*/

      //reg[r2] = reg[r1]
    case movl_reg_reg: //opcode: 5
      
      registers[instr.second_register] = registers[instr.first_register];
      break;


      //reg[reg2] = memory[reg[reg1] + imm]
    case movl_deref_reg: //opcode: 6
      
      //Read the bits from the given address as an integer, store in reg 2.    
      registers[instr.second_register] = *((int*) & memory[registers[instr.first_register] + instr.immediate]);
      break;


      //memory[reg[reg2] + imm] = reg[reg1]
    case movl_reg_deref: //opcode: 7    
      
      //Write the bits of reg 1 into the memory address by casting as an int*
      *((int*) & memory[registers[instr.second_register] + instr.immediate]) = registers[instr.first_register];
      break;


      //sign extend by ORing with 0xffff0000 if the imm. is negative
    case movl_imm_reg: //opcode: 8
      
      if (instr.immediate < 0)
	reg_num = SIGN_EXT_MASK | instr.immediate;
      else
	reg_num = 0x0 | instr.immediate;

      registers[instr.first_register] = reg_num;
      break;


      //OF: bit 11, SF: bit 7:, ZF: bit 6, CF: bit 0
    case cmpl: //opcode: 9  reg2 - reg1
      
      *eflags = 0;
      reg1 = registers[instr.first_register];
      reg2 = registers[instr.second_register];
    
      //set carry flag if reg1 is larger than reg2 in an unsigned interpretation
      if (((unsigned int) reg1) > ((unsigned int) reg2))
	*eflags |= 0x1;

      //set zero flag if r1 == r2
      if (reg1 == reg2)
	*eflags |= (0x1 << 6);

      //set sign flag if the most significant bit of reg2-reg1 is 1
      if ((reg2 - reg1) < 0)
	*eflags |= (0x1 << 7);

      //set overflow flag if either: (reg1 is +, reg2 is -, reg2-reg1 is +) OR (reg1 is -, reg2 is +, reg2-reg1 is -)
      if ((reg1 > 0) && (reg2 < 0) && (reg2 - reg1 > 0) || (reg1 < 0) && (reg2 > 0) && (reg2 - reg1 < 0))
	*eflags |= (0x1 << 11);
   
      break;


      /*Jump operations*/

      //jump if ZF set
    case je: //opcode: 10

      if ((*eflags >> 6) & 1)
	program_counter += instr.immediate;
      break;


      //jump if SF xor OF
    case jl: //opcode: 11

      if(((*eflags >> 7) & 1) ^ ((*eflags >> 11) & 1))
	program_counter += instr.immediate;
      break;

    
      //jump if (OF xor SF) or ZF
    case jle: //opcode: 12

      if ((((*eflags >> 11) & 1) ^ (((*eflags >> 7) & 1)) || ((*eflags >> 6) & 1)))
	program_counter +=  instr.immediate;
      break;


      //jump if not(SF xor OF)
    case jge: //opcode: 13
    
      if (!(((*eflags >> 7) & 1) ^ ((*eflags >> 11) & 1)))
	
	program_counter += instr.immediate;
      break;


      //jump if CF or ZF
    case jbe: //opcode: 14
    
      if((*eflags & 1) || ((*eflags) >> 6) & 1)
	program_counter += instr.immediate;
      break;


      //unconditionally jump by offset in immediate
    case jmp: //opcode: 15
 
      program_counter +=  instr.immediate;
      break;

   

      /*Calling/returning/stack operations*/

      //decrement stack pointer, store the program counter on the stack, jump by immediate offset
    case call: //opcode: 16
      
      *esp -= 4;
      *((int*) & memory[*esp]) = program_counter + 4;
      program_counter += instr.immediate;
    
      break;
    
   
      //if the stack pointer is outside of memory range, exit
      //otherwise, retrive program counter from the stack, increment stack pointer and return old pc
    case ret: //opcode: 17
    
      if (*esp == 1024)
	exit(0);
    
      else{
	program_counter = *((int*) &memory[*esp]);
	*esp += 4;
	return program_counter;
      }

      //decrement stack pointer to make room and store reg[r1] on the stack in memory
    case pushl: //opcode: 18
    
      *esp -= 4;
      *((int*) &memory[*esp]) = registers[instr.first_register];
      break;

      //retrieve the value in memory[*esp] and store in reg[r1], pop the stack by adding 4 to *esp
    case popl: //opcode: 19
     
      registers[instr.first_register] = *((int*) &memory[*esp]);
      *esp += 4;
      break;



      /*IO Operations*/

    
      //print (testing)
    case printr: //opcode: 20
      
      printf("%d (0x%x)\n", registers[instr.first_register], registers[instr.first_register]);
      break;

    
      //scan (testing)
    case readr: //opcode: 21
      
      scanf("%d", &(registers[instr.first_register]));
      break;

    }


  // program_counter + 4 represents the subsequent instruction
  return program_counter + 4;
}



/*********************************************/
/****  DO NOT MODIFY THE FUNCTIONS BELOW  ****/
/*********************************************/

/*
 * Returns the file size in bytes of the file referred to by the given descriptor
*/
unsigned int get_file_size(int file_descriptor)
{
  struct stat file_stat;
  fstat(file_descriptor, &file_stat);
  return file_stat.st_size;
}

/*
 * Loads the raw bytes of a file into an array of 4-byte units
*/
unsigned int* load_file(int file_descriptor, unsigned int size)
{
  unsigned int* raw_instruction_bytes = (unsigned int*)malloc(size);
  if(raw_instruction_bytes == NULL)
    error_exit("unable to allocate memory for instruction bytes (something went really wrong)");

  int num_read = read(file_descriptor, raw_instruction_bytes, size);

  if(num_read != size)
    error_exit("unable to read file (something went really wrong)");

  return raw_instruction_bytes;
}

/*
 * Prints the opcode, register IDs, and immediate of every instruction, 
 * assuming they have been decoded into the instructions array
*/
void print_instructions(instruction_t* instructions, unsigned int num_instructions)
{
  printf("instructions: \n");
  unsigned int i;
  for(i = 0; i < num_instructions; i++)
  {
    printf("op: %d, reg1: %d, reg2: %d, imm: %d\n", 
	   instructions[i].opcode,
	   instructions[i].first_register,
	   instructions[i].second_register,
	   instructions[i].immediate);
  }
  printf("--------------\n");
}

/*
 * Prints an error and then exits the program with status 1
*/
void error_exit(const char* message)
{
  printf("Error: %s\n", message);
  exit(1);
}
