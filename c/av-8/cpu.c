#include <stdio.h>
#include <stdlib.h>
//#include "instr.h"

#define ROMNAME "rom.bin"

#define DEBUG    1 
#define RAMEND   0xFFFF
#define FLASHEND 0xFFFF
#define FLASHSIZE FLASHEND 

/* Status Register */
struct SR 
{
  unsigned int C:1; /* Carry flag */
  unsigned int Z:1; /* Zero flag */ 
  unsigned int N:1; /* Negative flag */
  unsigned int V:1; /* Twos complement overflow indicator */
  unsigned int S:1; /* N^V (For signed tests) */
  unsigned int H:1; /* Half carry flag */
  unsigned int T:1; /* Transfer bit (BLD/BST) */
  unsigned int I:1; /* Global interrupt enable|disable */
};

struct SR SREG;

/* Instruction macros */
#define DEC(I) \
   (R[I] & 0x8000) ? (SREG.C=1) : (0) ; \
    R[I]--; \
   (R[I] == 0 ) ? (SREG.Z=1) : (SREG.Z=0) ; \
   (R[I] & 0x8000) ? (SREG.N=1) : (SREG.N=0) ; \
   (SREG.S = SREG.N ^ SREG.V) ; \
   PC++; \

#define NOP() PC++

/* Flash - Program Memory */
unsigned short FLASH[0xFFFF];

/* Data Memory */
unsigned short R[32];        /* 32 Registers */
unsigned short IO[64];       /* 64 Locations */
unsigned char  DATA[0xFFFF]; /* 64k          */

/* RO Registers */
unsigned short SP;
unsigned short PC;

void loadRom();
void printRegs();
void printFlags();

int main() {

    register unsigned int i;

    /* Initialize all registers to 0 */
    SP = 0;
    PC = 0;

    /* Load the ROM */
    loadRom();

    /* Start the CPU */
    do {

        if( FLASH[PC] == 0 )
            NOP();

        if( (FLASH[PC] & 0xFE0F) == 0x940A ) /* 1001010-----1010 */
            DEC( FLASH[PC] & 0x1F0 );
 
        if(DEBUG) {
            printf("Executed instruction: %x\n", FLASH[PC]);
            printRegs();
            printFlags();
        }
 
    } while ( PC < FLASHEND ); 

    printf("CPU Has Executed Flash Contents.\n");  

    return 0;

}

void loadRom() {

    FILE *rom_file;
    unsigned int len;

    rom_file = fopen(ROMNAME, "rb");

    if (rom_file) {
      
        len = fread(FLASH, 2, FLASHSIZE, rom_file);
      
        if (len < FLASHSIZE) {
          
            printf("ROM size mismatch, expected %d, got %d.\n", FLASHSIZE, len);
            exit(1);
    
        } 
    
    } else {

        printf("Error opening rom file.\n");
        exit(1);
  
    }  

}

void printRegs() {

    register int i;

    printf("General purpose registers:\n");

    for( i=0; i <= 31; i++ ) 
    {
       printf("R[%u]:\t%u\t", i, R[i]);
        
       if( (i+1) % 4 == 0 ) 
       {
           //printf("%d",i);
           printf("\n");
       }
    }
    printf("\n");
}

void printFlags() {

    register int i;

    printf("SREG:\n");
    printf("C:%d Z:%d N:%d V:%d\n", SREG.C, SREG.Z, SREG.N, SREG.V);
    printf("S:%d H:%d T:%d I:%d\n", SREG.S, SREG.H, SREG.T, SREG.I);
    printf("\n");
}
