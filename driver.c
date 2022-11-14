#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

void printadd(void* ptr){
  printf("the address is %d\n", ptr);
}

int main(int argc, char** argv){
  myinit(0);
  char* test = alloc(5);
  strcpy(test, "word");
  printf("test contents: %s\n", test);
  char* second = alloc(9);
  printadd(second);
  myfree(second);
  myfree(second);
  dump_heap();
  return 0;
}