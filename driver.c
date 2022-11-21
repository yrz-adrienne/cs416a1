#include <stdio.h>
#include <string.h>
#include "umalloc.h"

void printadd(void* ptr){
  printf("the address is %p\n", ptr);
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

  int point_test = 42;
  free(&point_test); 
  free((int*)point_test); 
  // printf("int cast to pointer size: %ld\n",sizeof((int *) point_test)); 
  // printf("int cast to pointer address: %p\n",(int *) point_test); 
  // printf("int max cast to pointer address: %p\n",(int *) __INT_MAX__); 
  // printf("unsigned 64 bit long int max cast to pointer address: %p\n",(int *) __UINT64_MAX__); 
  // printf("address of int variable: %p\n", &point_test); 
  // printf("address of int variable converted to int %ld", (unsigned long int)&point_test); 
  // printf("attempt to dereference int cast to pointer (seg fault): %d\n",*((int *) point_test)); 

  return 0;
}