#ifndef mymalloc
#define mymalloc

#include "./mymalloc.c"
void myinit(int allocAlg);
void* alloc(size_t size);
void myfree(void* ptr);

void dump_heap();

#endif