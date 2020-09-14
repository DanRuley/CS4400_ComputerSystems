Due Sep 12 by 11:59pm Points 100 Submitting a file upload File Types c Available Aug 26 at 12am - Sep 15 at 11:59pm 21 days
The objective of this assignment it to implement a simulator for a simplified x86 processor.  The basic job of a simulator is to mimic a real processor by keeping track (in software) of the various components of a processor such as registers, memory, and which instruction to execute next.  As your simulator program runs, it should "execute" simulated x86 instructions one at a time, while updating software representations of registers and memory.

This assignment is divided into two parts.  Student should get started on Part 1 right away.  More information about x86 instructions is covered in Lecture 3, which is useful for completing Part 2.

In order to complete such a simulator in our first assignment, the task is made easier by supporting only 32-bit ("l") instructions with a machine code representation of instructions fixed to the 32-bit size.

Start by downloading simulator.tar.gz, which is a gzipped tar file that can be unpacked by running the following command on a CADE lab1 machine:

tar zxvf simulator.tar.gz
This creates a new directory called simulator containing these files and directories:

simulator.c — This source code file serves as a starting point for your solution.
instruction.h — This header file provides some useful definitions for representing instructions.
tests — This directory contains simple, moderate, and complex tests for your simulator.  The tests themselves are machine code programs that your simulator should be able to input and run.
run_tests.sh — This bash script runs all of the tests and reports the results.
assembler — This file is a pre-compiled executable that can be used to generate a simulator input machine code file given an assembly file.  (You do not need to use this executable, but it may be useful for testing if you want to modify or write your own assembly test programs.)
Compiling the simulator
To compile the simulator, run one of the following commands:

gcc -g simulator.c -o simulator
The above compiles with debug symbols, useful during development.

gcc -O2 simulator.c -o simulator
The above compiles with optimization.  The second version is used for grading — make sure your program passes all tests when compiled with optimization.  If your program behaves differently with and without optimization, it most likely means your program has undefined behavior, such as using uninitialized data. 

Running the simulator
To run the simulator, pass it a command-line argument specifying the machine code binary file to run.  The tests directory contains a number of assembly files with extension .s and assembled binary machine code files with extension .o. The simulator accepts the .o files as input.

For example, you can run the simulator with the following command:

./simulator tests/simple/subl.o
This causes the simulator to load the subl.o binary file, which represents the machine encoding of the instructions in the corresponding .s file.

The simulator, as is provided above, only loads the binary file.  It is your job to finish the implementation of the simulator according to the specifications described in this assignment so that it executes (simulates) the provided machine code program.

Part 1 — Decode the Binary Machine Code
The provided .o files are binary files representing machine code for our simulator.  Each instruction is encoded as four consecutive bytes (32 bits).  For example, consider tests/simple/subl.s and tests/simple/subl.o.  Notice that there are four instructions in the assembly file, so the binary .o file should contain sixteen bytes (four instructions * four bytes each).  Verify that this is true by looking at the file size using the ls command:

ls -l tests/simple/subl.o
You should see "16" somewhere in the output.

Our first task is to decode the raw bytes into usable data structures in our simulator.  For Part 1, we decode the bytes.  In Part 2, we execute the decoded instructions.

Each 4-byte value in the machine code represents all of the necessary information for one instruction, such as the opcode and inputs/outputs.  Our machine code represents all instructions in the form given below.  All bits are specified in order of significance.

Bits:	31 - 27	26 - 22	21 - 17	16	15 - 0
Meaning:  	opcode	reg1	reg2	unused	immediate
For example, consider the third instruction in the test file tests/simple/addl_imm_reg.s:

addl    $-2, %ebx
In this particular form of the addl instruction, the opcode is 2, register %ebx has an ID of 1, and the immediate bits represent -2.  Thus, the instruction is encoded with the following bits:

00010000010000001111111111111110

opcode = 2

reg1 = 1

reg2 = 0 (unused)

immediate = -2 (16-bit signed two's complement)

The "reg2" bits are unused in this instruction since it has only one register.  The unused 0 bit between the reg2 and immediate bits is always unused.  See the full list of opcodes and register IDs.

instruction_t
Consider the provided instruction.h header file, which defines a struct called instruction_t for representing an instruction.  Our task is to create an array of instruction_t elements and fill in the fields of each element with the appropriate values based on the raw bits encoded in the input file.

For example, suppose that you create an instruction_t variable called instr, for representing the example addl instruction above.  After filling in the fields of instr, they should have the following values:

instr.opcode = 2

instr.first_register = 1

instr.second_register = 0

instr.immediate = -2
Your job is to programmatically extract these values out of the 32 bits that encode the instruction using bitwise shifting and masking operations in C.  To print the decoded values and check your work, uncomment line 75 in simulator.c.  Verify correctness using the opcode and register ID tables.

Example run
When the task of decoding instructions is complete for Part 1, run your program on the provided .o files.  As an example, the output of the simulator on tests/complex/factorial.o should be the following:

./simulator tests/complex/factorial.o
instructions: 
op: 0, reg1: 6, reg2: 0, imm: 8
op: 21, reg1: 5, reg2: 0, imm: 0
op: 16, reg1: 0, reg2: 0, imm: 12
op: 20, reg1: 0, reg2: 0, imm: 0
op: 2, reg1: 6, reg2: 0, imm: 8
op: 17, reg1: 0, reg2: 0, imm: 0
op: 18, reg1: 1, reg2: 0, imm: 0
op: 5, reg1: 5, reg2: 1, imm: 0
op: 8, reg1: 8, reg2: 0, imm: 2
op: 9, reg1: 8, reg2: 5, imm: 0
op: 10, reg1: 0, reg2: 0, imm: 16
op: 0, reg1: 5, reg2: 0, imm: 1
op: 16, reg1: 0, reg2: 0, imm: -28
op: 3, reg1: 1, reg2: 0, imm: 0
op: 15, reg1: 0, reg2: 0, imm: 4
op: 8, reg1: 0, reg2: 0, imm: 2
op: 19, reg1: 1, reg2: 0, imm: 0
op: 17, reg1: 0, reg2: 0, imm: 0
--------------
Re-comment line 75 before you run tests on Part 2.  The extra output causes the auto-tester to report failure.

Part 2 — Simulate the Assembly Instructions
With the usable array of instruction_t elements created in Part 1, the task in Part 2 is to simulate the execution of each instruction, one by one.  Your simulator must represent a program counter, registers, and a small memory stack.  Each instruction may modify registers and/or memory, and then control moves to the next instruction (by modifying the program counter).

The simulated machine should adhere to the following specifications:

The program counter should start at 0.  The first instruction to execute is always the first one in the input file.  Four bytes is sufficient to represent the program counter for any possible example.

There is a 1024-byte memory stack.  Every byte in memory should initially have a value of 0.

There are 17 registers (see the register ID table).  The width of each register is 32 bits.  All registers should initially have a value of 0, except %esp.  %esp is the stack pointer register, and should initially have a value of 1024.  (Note that address 1024 is just beyond the range of valid memory addresses.)

The simulator should execute instructions one by one using the behavior in the instruction definition table. Unless an instruction specifically modifies the program counter (e.g., jmp), the program counter should increase by 4 after executing an instruction.

There are two stopping conditions to reach the successful end of the simulated program:

The program_counter reaches 4 bytes past the last instruction address (i.e., the last instruction is executed, and it is not a jump/branch). 

The ret instruction is executed while the value of %esp is 1024.

Upon successful completion of the simulation, the simulator should return 0 exit status.

Instruction definitions
See the instruction definition table  for a list of all behaviors you must implement in Part 2.  Do not be daunted by the size of this table.  Once you have the infrastructure for executing one instruction, many of the remaining instructions execute with minor changes.  A few of the instructions are more challenging.

Additional Rules
In addition to the specifications given above, your solution must adhere to these rules:

Your simulator program must be pure C code, and you may not use any inline assembly.
You may not include any additional header files.
Your simulator may not rely on any undefined C behavior, especially in implementing the cmpl instruction.
Testing
Each of the provided tests (in the tests directory) has 3-5 associated files:

testname.s — the assembly code used to produce the .o file
testname.o — the binary machine code used as input to the simulator
testname.expected — the exact output expected of the simulator
testname.in (optional) — some tests also have an input file, which is used as the input for tests that use the readr instruction.
testname.c (optional) — some tests have an associated C source file, which shows the C code used to compile the assembly, for reference purposes only
The provided run_tests.sh bash script runs your simulator on all of the provided tests in the tests directory, compares the output to the expected output,  and print how many tests passed.  The test script requires that your executable be called "simulator".

The tests in the tests/complex directory all use the readr instruction to get input.  If you run the simulator manually on these tests, it looks like they are hung until you type in some inputs.  See tests/complex README for the meaning of the input.  The run_tests.sh script automatically provides input for these tests.

Modifying Tests
It may be useful for you to write your own tests or modify existing tests (such as adding printr instructions for debugging).  If you do this, do not modify the original file in the tests directory.  Instead, copy it elsewhere, then use the provided assembler tool to assemble it into a binary file.

Assembler
Use of the assembler is not required, but is helpful for debugging so you can write your own test programs.

The following commands depend on which shell your connection to the CADE lab is using. To find out, run: ps -p $$ while logged in to a CADE lab machine.

To run the assembler, first run this command in your CADE terminal.

For tcsh:

setenv LD_LIBRARY_PATH /usr/local/stow/gcc/amd64_linux26/gcc-4.9.4/lib64
For bash or zsh:

export LD_LIBRARY_PATH=/usr/local/stow/gcc/amd64_linux26/gcc-4.9.4/lib64/
You need to run this only once each time you login, then you can run the assembler with:

./assembler <input> <output>
The executable assembler expects two command-line arguments.  The first is the name of the input assembly file, and the second is the name of the output binary file it should produce. 

Assembly Format
The assembler is quite fragile, and requires the input assembly to adhere to specific formatting:

Labels are at the beginning of the line, and end with a colon.  Labels can be any number of letters followed by a digit.  Labels can optionally start with a '.' character.
Instruction lines begin with one tab character, followed by the instruction name.
Following the name is a tab character (except for the "ret" instruction).
Following the tab character are the arguments.
Arguments are separated by a comma and a space ", ".
Immediate arguments start with a dollar sign "$".
Register arguments start with a percent sign "%".
The forms of the movl instruction that use an offset require the offset right before the opening parenthesis.
In general, follow the formatting of the provided examples carefully.
Hints
Be careful when accessing the stack memory.  Its type is a byte array, but we are storing and retrieving 4-byte values from it.  Make sure to do proper casting when accessing it with pointers.

The names of registers are not important.  Represent them as an array, and simply index into that array using the register number encoded in the instruction.  The only special registers you need to explicitly access by name are %esp and %eflags.

The provided tests are categorized by simple, moderate, and complex.  It is recommended you get all simple tests working first, and so on.  The run_tests script runs them in roughly increasing order of difficulty.  Most tests require support for multiple instructions.

If you are worried about endianness, you are over-complicating things.  Instruction fields are encoded based on order of significance in a 32-bit value, not by order of byte address.  This is why the provided starting code reads the raw bytes into 32-bit (4-byte) data types.  Furthermore, storing and retrieving 4-byte values in the stack memory works correctly using the default behavior in C. 

Grading
Your simulator will be compiled with -O2 and the provided instruction.h.  Therefore, your entire solution must be contained in exactly one file: simulator.c.  If you modify instruction.h, your solution will not compile and will receive no credit.

The moderate tests are worth 2x as much as the simple tests.  The complex tests are worth 3x as much as the simple tests.

Submission
Submit exactly one file simulator.c here.  All CS 4400 assignments are graded/evaluated on the CADE lab1 machines.  Even if your solution works on some other machine, it will receive no credit if it does not work on CADE lab1.












Assignment 1: Instruction opcodes
The following table gives the full set of instructions for our simulator, with corresponding opcodes:

Assembly Name	Instruction            	Opcode 
subl	subl	0
addl	addl_reg_reg	1
addl	addl_imm_reg	2
imull	imull	3
shrl	shrl	4
movl	movl_reg_reg	5
movl	movl_deref_reg	6
movl	movl_reg_deref	7
movl	movl_imm_reg	8
cmpl	cmpl	9
je	je	10
jl	jl	11
jle	jle	12
jge	jge	13
jbe	jbe	14
jmp	jmp	15
call	call	16
ret	ret	17
pushl	pushl	18
popl	popl	19
printr	printr	20
readr	readr	21
Notice that two assembly instructions have multiple different forms, addl and movl.  The various forms of these instructions have the same name in the assembly file, but they have different opcodes based on the inputs/outputs they use.  

For example, this instruction moves a register into a register (opcode 5):

movl    %eax, %ebx
This instruction moves an immediate into a register (opcode 8): 

movl    $1, %eax
This instruction moves a register into memory at the address pointed to by %eax + 0 (opcode 7):

movl    %ebx, 0(%eax)
Notice that not all instructions use two registers and not all instructions use an immediate.  For such instructions, the unused components have a value of 0.  Thus, all instructions can be decoded using the same logic, and you do not need to write any special cases based on the opcode — simply extract the bits into the associated fields in an instruction_t struct.







Assignment 1: Register IDs
The following table gives the full set of registers for our simulator, with corresponding IDs:

Register 	ID
%eax	0
%ebx	1
%ecx	2
%edx	3
%esi	4
%edi	5
%esp	6
%ebp	7
%r8d	8
%r9d	9
%r10d	10
%r11d	11
%r12d	12
%r13d	13
%r14d	14
%r15d	15
%eflags	16






Assignment 1: Instruction definitions
The table below gives the full set of instructions for our simulator and defines the behavior for each.  Note that an 'x' in the reg1, reg2, or imm column indicates that those fields are used.

Name	opcode	reg1	reg2	imm	Example	Function
subl	0	x		x	subl $8, %esp	reg1 = reg1 - imm
addl	1	x	x		addl %edi, %edx	reg2 = reg2 + reg1
addl	2	x		x	addl $8, %esp	reg1 = reg1 + imm
imull	3	x	x		imull %edx, %eax	reg2 = reg1 * reg2
shrl	4	x			shrl %edi	reg1 = reg1 >> 1 (logical shift)
movl	5	x	x		movl %eax, %esi	reg2 = reg1
movl	6	x	x	x	movl 20(%esp), %eax	reg2 = memory[reg1 + imm]   (moves 4 bytes)
movl	7	x	x	x	movl %eax, 20(%esp)	memory[reg2 + imm] = reg1   (moves 4 bytes)
movl	8	x		x	movl $6, %esi	reg1 = sign_extend(imm)
cmpl	9	x	x		cmpl %ebx, %ecx	
perform reg2 - reg1, set condition codes 

does not modify reg1 or reg2

je	10			x	je foo	jump on equal
jl	11			x	jl foo	jump on less than
jle	12			x	jle foo	jump on less than or equal
jge	13			x	jge foo	jump on greater than or equal
jbe	14			x	jbe foo	jump on below or equal (unsigned)
jmp	15			x	jmp foo 	unconditional jump
call	16			x	call foo	
%esp = %esp - 4

memory[%esp] = program_counter + 4

jump to target

ret	17				ret	
if %esp == 1024, exit simulation

else

program_counter = memory[%esp]

%esp = %esp + 4

pushl	18	x			pushl %ebx	
%esp = %esp - 4

memory[%esp] = reg1

popl	19	x			popl %ebx	
reg1 = memory[%esp]

%esp = %esp + 4

printr	20	x			printr %eax	print the value of reg1
readr	21	x			readr %eax	user input an integer to reg1
 

Condition Codes
The cmpl instruction sets the condition codes (CF, ZF, SF, OF) corresponding to certain bits in the %eflags register.  The conditional jump instructions read these codes and either branch to the target or move to the subsequent instruction, based on the appropriate flags.

When the cmpl instruction is executed, reg2 - reg1 is performed to determine these condition codes:

Condition	%eflags bit
CF (carry)	
If reg2 - reg1 results in unsigned overflow, CF is true

0

ZF (zero)	If reg2 - reg1 == 0, ZF is true	6
SF (sign)	If most significant bit of (reg2 - reg1) is 1, SF is true	7
OF (overflow)	
If reg2 - reg1 results in signed overflow, OF is true

11

These codes are stored using certain bits of the %eflags register.  For example, consider the following sequence of instructions:

movl    $1, %eax
movl    $2, %ebx
cmpl    %ebx, %eax
After execution of the cmpl instruction, the %eflags register should contain the value 129 (0x81) because:

The result of %eax - %ebx is 0xFFFFFFFF.
The most significant bit is set (SF).
Unsigned overflow occurred (CF).
A number with the seventh bit (SF) and zeroth bit (CF) set is 128 + 1 = 129.
 

Jumping/Branching Offsets
je, jl, jle, jge, jbe, jmp, and call instructions use an immediate offset, which is the target instruction address relative to the next instruction's address.

For example, consider these instructions:

start:
    jl label1
    printr %eax
label1:
    printr %ebx
    jmp start
    ret
The address of the "start" label (the first jl instruction) is 0.  The address of "label1" is 8.

The immediate value contained in the jl instruction is 4 because "label1" starts 4 bytes away from the next instruction after jl.

The immediate value contained in the jmp instruction is -16, since the "star"t label is -16 bytes away from the next instruction after jmp.

 

Conditional Jumps
The je, jl, jle, jge, and jbe instructions are conditional jumps.  This means they move to the next instruction (program_counter + 4) if the condition is false, or they jump to their target offset if the condition is true.

The conditions are as follows:

Jump Type  	Jump If
je	ZF
jl	SF xor OF
jle	(SF xor OF) or ZF
jge	not (SF xor OF)
jbe	CF or ZF
 

I/O instructions
There are two special (fake) instructions to help with I/O:

readr (read register)
printr (print register)
The course staff will primarily use printr to determine if your simulator is correct.  These instructions are already implemented for you.
