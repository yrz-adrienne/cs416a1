CC = gcc
CFLAGS = -g -c
AR = ar -rc
RANLIB = ranlib

all: umalloc.a driver memgrind

umalloc.a: umalloc.o 
	$(AR) libumalloc.a umalloc.o 
	$(RANLIB) libumalloc.a

memgrind:
	$(CC) -o memgrind memgrind.c -L./ -lumalloc 

umalloc.o: umalloc.h
	$(CC) $(CFLAGS) umalloc.c

driver: 
	$(CC) -o driver driver.c -L./ -lumalloc


clean:
	rm -rf driver memgrind *.o *.a