CC = gcc
CFLAGS =  -fPIC -Wall -lpthread
LDFLAGS =  -fPIC -pthread
RM = rm -f
TARGET_LIB = libfifo.so

SRCS = fifo.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} ${SRCS.c=.d}