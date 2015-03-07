#include <stdio.h>

struct optable {
  char code[10];
   char objcode[10];
} optab[1] = {
  {"NOP","00"}
};

void usage(char *name) {

  printf("Usage: %s prog.asm\n", name);

}

int main(int argc, char *argv[]) {

  if( argc < 2 ) 
  {
    usage(argv[0]);
    return -1;
  }

  printf("arg: %s\n", argv[1]);  
  return 0;

}
