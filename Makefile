CC = gcc
CCFLAGS = -O -c
LIBS = -lpthread

all: libfifo.a
	
fifo.o: fifo.c fifo.h
	gcc -O -c fifo.c

libfifo.a: fifo.o
	ar rcs libfifo.a fifo.o

libs: libmylib.all

clean:
	rm -f *.o *.a *.gch