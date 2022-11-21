#include "umalloc.h"
#include <signal.h>

//original setup
const int shift = 31; //using unsigned int
unsigned int taken = 0b1 << shift;

static char mem[MEM_SIZE]; 
char init = '\0'; 
int s_line; 
char * s_file; 

unsigned int rem(unsigned int x){

}

void sigsegv_handler(){
  printf("Error on free: attempted to free a invalid pointer from line %d, file %s\n", s_line, s_file); 
  exit(-1); 
}

void myinit(){
  unsigned int *p = (unsigned int*)&mem; 
  p[0] = (MEM_SIZE - 4); //set the size of the block to the total size - 1 metadata
  //set the most significant bit to 1
  init = 'y'; 
  if(DEBUG){
    printf("initialized\n"); 
  }
  signal(SIGSEGV, sigsegv_handler); 
}

void print_header(int i) {
  unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
  unsigned int chunk_size_bytes = *curr_chunk & (~taken);; 
  unsigned int chunk_taken = (*curr_chunk & taken) >> shift;
  printf("%p - size: %d, allocated: %c \n", curr_chunk, chunk_size_bytes, (chunk_taken ? 'y' : 'n'));  
}

int isallocated(int i) {
  unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
  unsigned int chunk_taken = (*curr_chunk & taken) >> shift; 
  return chunk_taken;
}

unsigned int chunksize(int i) {
  unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
  unsigned int chunk_size_bytes = *curr_chunk & (~taken);; 
  return chunk_size_bytes; 
}

unsigned int* getchunk(int i) {
  return (unsigned int *)&mem[i];  
}

void memdump(){
  int i = 0;
  printf("...\nmemory dump\n"); 
  while(i<MEM_SIZE){
      unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
      unsigned int chunk_size_bytes = *curr_chunk & (~taken);; 
      unsigned int chunk_taken = (*curr_chunk & taken) >> shift;
      printf("%p - size: %d, allocated: %c \n", curr_chunk, chunksize(i), isallocated(i) ? 'y' : 'n'); 
      i += chunk_size_bytes + 4;
  }
}

// take an integer from 0 to MAX_INT, return a pointer
void* umalloc(size_t bytes, char* file, int line){
  if(bytes == 0 || bytes > (MEM_SIZE) - 4){
    // return null if the size is 0 or if more bytes than possible are requested
    printf("Error on malloc: The amount requested is impossible (0 or > memory size) from line %d, file %s\n", line, file); 
    return NULL;
  }
  else{
    // initialize if we haven't yet
    if(init=='\0'){
      myinit(); 
    }

    unsigned int free_mem = 0;
    int currIndex = 0;
    while(currIndex<MEM_SIZE){
      unsigned int *curr_chunk = getchunk(currIndex);
      unsigned int chunk_size_bytes = chunksize(currIndex);
      unsigned int chunk_taken = isallocated(currIndex); 
      if(!chunk_taken){
        if(chunk_size_bytes >= bytes){
          unsigned int rem_size = chunk_size_bytes - (bytes + 4); 
          if(rem_size > 0 && rem_size < chunk_size_bytes){ // underflow case
            if (DEBUG) {
              printf("splitting the block\n");
            }
            // if we can break the block into two chunks 
            // then set up the next chunk
            *curr_chunk = taken | bytes;  // current one is in use
            unsigned int* next_chunk = getchunk(currIndex + bytes + 4);  // the next block that can be used
            *next_chunk = rem_size;
          }
          else{
            if (DEBUG) {
              printf("not splitting\n");
            }
            // not big enough of a space to split into two chunks
            *curr_chunk = taken | chunk_size_bytes; //mark current as in use
          }
          return curr_chunk + 4; 
        }
        else{
          free_mem+=chunk_size_bytes; 
        }
      }
      // increment the index of the chunks to the next
      // size of the chunk in bytes + 4 for metadata
      currIndex += chunk_size_bytes + 4;
    }
    if(free_mem == 0){
      printf("Error on malloc: no free memory from line %d, file %s\n", line, file); 
    }
    else if(free_mem > bytes){
      printf("Error on malloc: there is enough free memory but no block large enough from line %d, file %s\n", line, file); 
    }
    else{
      printf("Error on malloc: memory is not full but not enough free memory from line %d, file %s\n", line, file); 
    }
    return NULL;
  }
}

// how would you detect if you tried to free something that's not a pointer??
void ufree(void* ptr, char * file, int line){
  s_file = file; 
  s_line = line; 
  unsigned int* target = ptr;
  target -= 4;
  if(ptr == NULL){
    printf("Error on free: attempted to free() a null pointer from line %d, file %s\n", line, file); 
    return;
  }
  unsigned int test = *target; //try to cause a seg fault
  int i = 0;
  int prev = -1; 
  while(i<MEM_SIZE){
    unsigned int *curr_chunk = (unsigned int *)&mem[i]; 
    unsigned int chunk_size_bytes = *curr_chunk & (~taken); 
    unsigned int chunk_taken = (*curr_chunk & taken) >> shift;
      if(target == curr_chunk){
        if(DEBUG){
          printf("match found at address: %p\n", ptr);
        }
        if(!chunk_taken){ // we cannot free unallocated memory
          printf("Error on free: attempted to free() an unallocated block from line %d, file %s\n", line, file); 
          return;
        }
        // mark the current block as unallocated
        *curr_chunk = chunk_size_bytes;

        

        // check if we are in bounds for the next segment
        if(i+chunk_size_bytes+4 < MEM_SIZE){
          unsigned int *next_chunk = (unsigned int *)&mem[i+chunk_size_bytes+4]; 
          unsigned int next_chunk_taken = (*next_chunk & taken) >> shift;
          unsigned int next_chunk_size = (*next_chunk & ~taken);
          if(!next_chunk_taken){
            //although it should be redundant to & with ~taken for chunks that aren't allocated
            *curr_chunk = chunk_size_bytes + next_chunk_size + 4; 
          }
        }

        chunk_size_bytes = *curr_chunk & (~taken); 
        if(prev != -1 && !isallocated(prev)){
          unsigned int* prev_chunk = getchunk(prev);
          *prev_chunk = chunksize(prev) + chunk_size_bytes + 4; 
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



