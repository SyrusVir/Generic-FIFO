CC := gcc
CCFLAGS := -g -O -c
LIBFLAGS := -lpthread
ARFLAGS := rcs

fifo.o: fifo.c fifo.h
	gcc $(CCFLAGS) fifo.c $(LIBFLAGS)

all: libfifo.a
	
libfifo.a: fifo.o
	ar $(ARFLAGS) libfifo.a fifo.o

.PHONY: clean
clean:
	rm -f *.o *.a *.gch