#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define malloc(X) umalloc(X, __FILE__, __LINE__)
#define free(Y) ufree(Y, __FILE__, __LINE__)

void myinit();
void memdump(); 
void * umalloc(size_t bytes, char *file, int line);
void ufree(void *ptr, char *file, int line);