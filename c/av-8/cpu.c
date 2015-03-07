#include <stdio.h>
#include <stdlib.h>
#include "lib/opmask.h"

#define ROMNAME "rom.bin"

#define DEBUG    1 
#define RAMEND   0xFFFF
#define FLASHEND 0xFFFF
#define FLASHSIZE FLASHEND 

struct CHIP 
{

  struct STATUS_REGISTER 
  {
    unsigned int C:1; /* Carry flag */
    unsigned int Z:1; /* Zero flag */ 
    unsigned int N:1; /* Negative flag */
    unsigned int V:1; /* Twos complement overflow indicator */
    unsigned int S:1; /* N^V (For signed tests) */
    unsigned int H:1; /* Half carry flag */
    unsigned int T:1; /* Transfer bit (BLD/BST) */
    unsigned int I:1; /* Global interrupt enable|disable */
  } SR;

  /* 32 General Purpose registers */
  unsigned short R[32];  

  /* X, Y, Z, Registers */
  unsigned short RX;
  unsigned short RY;
  unsigned short RZ;

  /* Flash - Program Memory */
  unsigned short FLASH[0xFFFF];

  /* Data Memory */
  unsigned short IO[64];       /* 64 Locations */
  unsigned char  DATA[0xFFFF]; /* 64k          */

  /* RO Registers */
  unsigned short SP;
  unsigned short PC;

};

void
DEC (struct CHIP *C, unsigned short I) 
{

   C->R[I] & 0x8000 ? (C->SR.V=1) : (C->SR.V=0); 
   C->R[I] & 0x8000 ? (C->SR.V=1) : (C->SR.V=0); 
   C->R[I]--; 
   C->R[I] == 0 ? (C->SR.Z=1) : (C->SR.Z=0); 
   C->R[I] & 0x8000 ? (C->SR.N=1) : (C->SR.N=0); 
   C->SR.S = C->SR.N ^ C->SR.V; 
   C->PC++;
 
}

void 
NOP (struct CHIP *C) 
{

  C->PC++;

}

void 
ADD (struct CHIP *C, unsigned short dst, unsigned short src) 
{

  C->R[dst] = C->R[src] + C->R[dst]; 
  C->R[dst] & 0x8000 ? (C->SR.N=1) : (C->SR.N=0); 
  C->R[dst] == 0 ? (C->SR.Z=1) : (C->SR.Z=0);
  C->SR.S = C->SR.N ^ C->SR.V;
  C->PC++; 

}

void 
ADC (struct CHIP *C, unsigned short dst, unsigned short src)
{

  unsigned short carry  = C->R[dst] & C->R[src];
  unsigned short result = C->R[dst] ^ C->R[src];
  unsigned short sc = 0;

  C->SR.H = (carry & 0x80) ? 1 : 0;

  while( carry != 0) {
    sc = carry << 1;
    carry = result & sc;
    result ^= sc;
  }

  C->R[dst] = result;

}

void flashRom(struct CHIP *);
void printRegs(struct CHIP *);
void printFlags(struct CHIP *);

int 
main () 
{

  struct CHIP chip;
  struct CHIP *C;
  register unsigned int i;

  C = &chip;

  /* Initialize all registers to 0 */
  C->SP = 0;
  C->PC = 0;

  /* Load the ROM onto the chip */
  flashRom(C);

  /* Start the CPU */
  do {

    if( C->FLASH[C->PC] == 0 )
      NOP(C);

    if( (C->FLASH[C->PC] & 0xFE0F) == 0x940A ) /* 1001010-----1010 */
      DEC( C, (C->FLASH[C->PC] & 0x1F0) );

//  if( )
//   ADD();
 
    if(DEBUG) {
      printf("Executed instruction: %x\n", C->FLASH[C->PC]);
      printRegs(C);
      printFlags(C);
    }
 
  } while ( C->PC < FLASHEND ); 

  printf("CPU Has Executed Flash Contents.\n");  

  return 0;

}

void flashRom(struct CHIP *C) {

    FILE *rom_file;
    unsigned int len;

    rom_file = fopen(ROMNAME, "rb");

    if (rom_file) {
      
        len = fread(C->FLASH, 2, FLASHSIZE, rom_file);
      
        if (len < FLASHSIZE) {
          
            printf("ROM size mismatch, expected %d, got %d.\n", FLASHSIZE, len);
            exit(1);
    
        } 
    
    } else {

        printf("Error opening rom file.\n");
        exit(1);
  
    }  

}

void printRegs(struct CHIP *C) {

    register int i;

    printf("General purpose registers:\n");

    for( i=0; i <= 31; i++ ) 
    {
       printf("R[%u]:\t%x\t", i, C->R[i]);
        
       if( (i+1) % 4 == 0 ) 
       {
           printf("\n");
       }
    }
    printf("\n");
    printf("SP: %x(%d) PC: %x(%d)\n", C->SP, C->SP, C->PC, C->PC);
}

void printFlags(struct CHIP *C) {

    register int i;

    printf("SREG:\n");
    printf("C:%x Z:%x N:%x V:%x\n", C->SR.C, C->SR.Z, C->SR.N, C->SR.V);
    printf("S:%x H:%x T:%x I:%x\n", C->SR.S, C->SR.H, C->SR.T, C->SR.I);
    printf("\n");
}
