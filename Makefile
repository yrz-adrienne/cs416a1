
CFLAGS = -g -Wall -Wvla -fsanitize=address

driver : driver.c	mymalloc.o
	gcc $(CFLAGS) -o driver driver.c 

%.o: %.c
	gcc -c -o $@ $<

