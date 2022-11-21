#include <stdio.h>
#include <string.h>
#include "umalloc.h"

void printadd(void* ptr){
  printf("the address is %d\n", ptr);
}

int main(int argc, char** argv){
  myinit();
  memdump();
  char* test = malloc(5);
  strcpy(test, "word");
  memdump();
  char* test2 = malloc(5);
  memdump();
  free(test);
  memdump();
  free(test2);
  memdump();


  return 0;
}