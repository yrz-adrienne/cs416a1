#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

void myinit(int allocAlg);
void myfree(void* ptr);
void dump_heap();

typedef struct {
  unsigned int upper;
  unsigned int lower;
} block;

block* heap;
int blocks = 30;
const int shift = 30;
int taken = 0b1 << shift;

void myinit(int allocAlg){
  block *data;
  data = (block *) malloc(blocks* sizeof(block));
  printf("size: %d\n", sizeof(block));
  for(int i=0;i<blocks;i++){
    data[i].upper = 0x0;
    data[i].lower = 0x0;
  }
  data[0].lower = (blocks-1)*8; // set the first one to open and have size of all bytes
  heap = data;
}

void* alloc(size_t size){

  if(size == 0){
    return NULL;
  }else{
    int i = 0;
    while(i<blocks){

      double double_size = (double) size;
      double chunk_size = 8.0;
      double chunks_needed_double = ceil(double_size / chunk_size);
      int chunks_needed = (int) chunks_needed_double;

      int chunk_size_bytes = heap[i].lower & (~taken);
      int chunk_taken = (heap[i].lower & taken) >> shift;

      if(!chunk_taken && chunk_size_bytes > size){
        // we return a pointer to the next spot and then update this header
        heap[i].lower = taken | size; // set the size of the block here
        int next_open = chunks_needed; // this is where the next uninterrupted block starts
        if(heap[i+next_open+1].lower == 0x0){
          int next_size = chunk_size_bytes - size - 8; // figure out the new size for this chunk-headers makeup the 2
          heap[i+next_open+1].lower = next_size;
        }
        return &heap[i+1].upper;
      }
      // increment the index of the chunks to the next

      double double_skip_size = (double) chunk_size_bytes;
      double chunks_skip_double = ceil(double_skip_size / chunk_size);
      int chunks_skip = (int) chunks_skip_double + 1;

      i += chunks_skip;
    }
    return NULL;

  }
}

void myfree(void* ptr){
  int target = ptr;
  printf("looking for address: %d\n", target);
  if(ptr == NULL){return;}
  int i = 0;
  while(i<blocks){
      int block_address = &heap[i+1].upper;
      printf("the address we are testing against is %d\n", block_address);
      int chunk_size_bytes = heap[i].lower & (~taken);
      double chunk_size = 8.0;

      if(target == block_address){
        // make the block at index i be unallocated
        printf("match found at address: %d\n", ptr);
        if((heap[i].lower & taken >> shift) == 0){ // we cannot free unallocated memory
          printf("trying to free unallocated memory\n");
          return;
        }

        // find the size of bytes until the next block
        double double_size = (double) chunk_size_bytes;
        double chunks_skip_double = ceil(double_size / chunk_size);
        int new_size = (int) chunks_skip_double * 8;
        heap[i].lower = new_size & (~taken);

        return;
      }
      // increment the index of the chunks to the next

      double double_skip_size = (double) chunk_size_bytes;
      double chunks_skip_double = ceil(double_skip_size / chunk_size);
      int chunks_skip = (int) chunks_skip_double + 1;

      i += chunks_skip;
    }
    return;

}

void dump_heap(){
  for(int i=0;i<blocks;i++){
    printf("%d 0x%08x%08x\n", &heap[i],heap[i].upper, heap[i].lower);
  }
  printf("\n");
}


