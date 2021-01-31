/**
 * Author: Jude Alnas
 * Date: Jan 30 2021
 * Description: A generic FIFO buffer. Buffers data pointers. Includes priority functionality
 * **/
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

typedef struct Node {
    void* data;
    struct Node *next;
    struct Node *prev;
    int priority;
} fifo_node_t;

typedef struct Buffer {
    pthread_mutex_t lock;
    pthread_cond_t cond_nonfull;
    pthread_cond_t cond_nonempty;
    int max_buffer_size;
    int buffer_occupancy;
    fifo_node_t *sentinel;
} fifo_buffer_t;

int fifoPush(fifo_buffer_t* buffer, void* data, int priority, bool blocking);
void* fifoPull(fifo_buffer_t* buffer, bool blocking);
void** fifoFlush(fifo_buffer_t* buffer, bool blocking);
fifo_buffer_t* fifoBufferInit(int max_buffer_size);
void** fifoBufferClose(fifo_buffer_t*);
void fifoPrint(fifo_buffer_t* buffer);
int fifoUpdateOccupancy(fifo_buffer_t* buffer);