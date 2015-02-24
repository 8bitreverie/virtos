#include <stdio.h>

#define DEBUG    1 
#define RAMEND   0xFFFF
#define FLASHEND 0xFFFF

/* Instruction macros */
#define NOP PC++

/* Status Register */
struct
{
  unsigned int C:1; /* Carry flag */
  unsigned int Z:1; /* Zero flag */ 
  unsigned int N:1; /* Negative flag */
  unsigned int V:1; /* Twos complement overflow indicator */
  unsigned int S:1; /* N^V (For signed tests) */
  unsigned int H:1; /* Half carry flag */
  unsigned int T:1; /* Transfer bit (BLD/BST) */
  unsigned int I:1; /* Global interrupt enable|disable */
} SREG;

/* Flash - Program Memory */
unsigned short FLASH[0xFFFF];

/* Data Memory */
unsigned short R[32];        /* 32 Registers */
unsigned short IO[64];       /* 64 Locations */
unsigned char  DATA[0xFFFF]; /* 64k          */

/* RO Registers */
unsigned short SP;
unsigned short PC;

int main() {

  /* Initialize all registers to 0 */
  SP = 0;
  PC = 0;

  /* Start the CPU */
  do {

    if( FLASH[PC] == 0 )
      NOP;
    
    if(DEBUG)
      printf("Executed instruction: %x\n", FLASH[PC]);
     
  } while ( PC < FLASHEND ); 

  printf("CPU Has Executed Flash Contents.\n");  

  return 0;

}

