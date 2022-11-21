#include "umalloc.c"
#include <time.h>
#include <string.h>

#define SAT_SIZE (MEM_SIZE - 9216*(1024+4))/5 + 9216 + 1
// this needs to store all the pointers in the saturation
static char* sat[SAT_SIZE];
int sat_i = 0; 
size_t max = MEM_SIZE - 4; 

void consistency(){
    char* first = (char*)malloc(7); 
    strcpy(first, "first!"); 
    free(first); 
    char* second = (char*)malloc(7); 
    printf("first: %p, second %p\n", first, second); 
    if(first == second){
        printf("consistency test: addresses are the same\n"); 
    }
}

void maximization(){
    size_t size = 1; 
    char * one = (char*)malloc(size); 
    while(one != NULL){
        free(one); 
        size = size*2; 
        one = (char*)malloc(size); 
    }
    while(one == NULL){
        size = size/2; 
        one = (char*)malloc(size); 
    }
    free(one);
    printf("maximization test: max is %zu \n", size); 
    max = size; 
}

void basic_coalescence(){
    int* half = (int*)malloc(max/2); 
    int* quarter = (int*)malloc(max/4); 
    free(half); 
    free(quarter); 
    int* max = (int*)malloc(max); 
    if(max != NULL){
        printf("basic coalescence test: success \n");
        free(max); 
    }
}

void saturation(){
    int i; 
    for(i=0; i< 9216; i++){
        sat[i] = (char*)malloc(1024); 
    }
    while(sat[i-1]!= NULL){
        sat[i] = (char*)malloc(1); 
        i++;
    }
    sat_i = i-2; 
}

void time_overhead(){
    free(sat[sat_i]); 
    clock_t start, end;
    start = clock(); // get the system time
    sat[sat_i] = (char*)malloc(1); 
    end = clock(); 

    printf("time overhead test: %f \n", ((double) (end - start)) / CLOCKS_PER_SEC); 
}

void intermediate_coalescence(){
    for(int i = 0; i <= sat_i; i++){
        free(sat[i]); 
    }
    char * max_alloc = malloc(max); 
    if(max_alloc != NULL){
        printf("intermediate coalescence test: success \n"); 
        free(max_alloc); 
    }
}

int main(int argc, char** argv){
    consistency();
    maximization();
    basic_coalescence();
    saturation();
    time_overhead();
    intermediate_coalescence();

    return 0;
}
