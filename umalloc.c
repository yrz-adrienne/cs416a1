#include "umalloc.h"

#define DEBUG 1
#define MEM_SIZE 10485760
// void myinit();
// void myfree(void* ptr);
// void dump_heap();

typedef struct {
  unsigned int upper;
  unsigned int lower;
} block;


//original setup
const int shift = 31; //using unsigned int
unsigned int taken = 0b1 << shift;

static char mem[MEM_SIZE]; 
char init = '\0'; 

unsigned int rem(unsigned int x){

}

void myinit(){
  unsigned int *p = (unsigned int*)&mem; 
  p[0] = MEM_SIZE - 4; //set the size of the block to the total size - 1 metadata
  //set the most significant bit to 1
  init = 'y'; 
  if(DEBUG){
    printf("initialized"); 
  }
}

void memdump(){
  int i = 0;
  printf("...\nmemory dump\n"); 
  while(i<MEM_SIZE){
      unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
      unsigned int chunk_size_bytes = *curr_chunk & (~taken);; 
      unsigned int chunk_taken = (*curr_chunk & taken) >> shift;
      printf("%p - size: %d, allocated: ", curr_chunk, chunk_size_bytes); 
      if(chunk_taken){
        printf("y\n"); 
      }
      else{
        printf("n\n"); 
      }
  }
}

// take an integer from 0 to MAX_INT, return a pointer
void* umalloc(size_t bytes, char* file, int line){
  if(bytes == 0 || bytes > (MEM_SIZE) - 4){
    // return null if the size is 0 or if more bytes than possible are requested
    return NULL;
  }
  else{
    if(init=='\0'){
      myinit(); 
    }

    int i = 0;
    while(i<MEM_SIZE){
      unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
      unsigned int chunk_size_bytes = *curr_chunk & (~taken);; 
      unsigned int chunk_taken = (*curr_chunk & taken) >> shift;

      if(!chunk_taken && chunk_size_bytes >= bytes){
        unsigned int rem_size = chunk_size_bytes - bytes - 4; 
        if(rem_size > 0){
          // if we can break the block into two chunks 
          // then set up the next chunk
          *curr_chunk = taken | bytes;  // current one is in use
          *(unsigned int*)&mem[i + bytes + 4] = rem_size; // next one is not in use
        }
        else{
          // not big enough of a space to split into two chunks
          *curr_chunk = taken | chunk_size_bytes; //mark current as in use
        }
        return &mem[i]; 
      }

      // increment the index of the chunks to the next
      // double double_skip_size = (double) chunk_size_bytes;
      // double chunks_skip_double = ceil(double_skip_size / chunk_size);
      // int chunks_skip = (int) chunks_skip_double + 1;
      i += chunk_size_bytes + 4;
    }
    return NULL;
  }
}

// how would you detect if you tried to free something that's not a pointer??
void ufree(void* ptr, char * file, int line){
  int target = ptr;
  printf("looking for address: %d\n", target);
  if(ptr == NULL){
    printf("Error on free: attempted to free() a null pointer from %d, file %s\n", line, file); 
    return;
  }
  int i = 0;
  int prev = -1; 
  while(i<MEM_SIZE){
    char * chunk_addr = &mem[i];
    unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
    unsigned int chunk_size_bytes = *curr_chunk & (~taken); 
    unsigned int chunk_taken = (*curr_chunk & taken) >> shift;

      if((char *)target == chunk_addr){
        if(DEBUG){
          printf("match found at address: %d\n", ptr);
        }
        if(!chunk_taken){ // we cannot free unallocated memory
          printf("Error on free: attempted to free() an unallocated block from line %d, file %s\n", line, file); 
          return;
        }
        // mark the current block as unallocated
        *curr_chunk = chunk_size_bytes;

        // on successful free, merge with unallocated blocks before or after
        unsigned int *prev_chunk = (unsigned int *)&mem[prev]; 
        unsigned int prev_chunk_taken = (*prev_chunk & taken) >> shift;
        if(i+chunk_size_bytes+4 < MEM_SIZE){
          unsigned int *next_chunk = (unsigned int *)&mem[i+chunk_size_bytes+4]; 
          unsigned int next_chunk_taken = (*next_chunk & taken) >> shift;
          if(!next_chunk_taken){
            //although it should be redundant to & with ~taken for chunks that aren't allocated
            *curr_chunk = chunk_size_bytes + (*next_chunk  & (~taken)) + 4; 
          }
        }
        if(prev && !prev_chunk_taken){
          *prev_chunk = (*prev_chunk  & (~taken)) + chunk_size_bytes; 
        }
        return;
      }
      // increment the index of the chunks to the next
      prev = i; 
      i += chunk_size_bytes + 4;
    }
    printf("Error on free: attempted to free() a pointer not obtained from malloc from line %d, file %s\n", line, file); 
    return;
}



