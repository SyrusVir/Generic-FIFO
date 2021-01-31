/**
 * Author: Jude Alnas
 * Date: Jan 30 2021
 * Description: A generic FIFO buffer. Buffers data pointers. Includes priority functionality
 * **/
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Node {
    int priority;
    void* data;
    struct Node *next;
    struct Node *prev;
} fifo_node_t;

typedef struct Buffer {
    pthread_mutex_t lock;
    pthread_cond_t cond_nonfull;
    pthread_cond_t cond_nonempty;
    int max_buffer_size;
    int buffer_occupancy;
    fifo_node_t *head;
    fifo_node_t *tail;
} fifo_buffer_t;

int fifoPush(fifo_buffer_t* buffer, void* data, int priority, bool blocking);
void* fifoPull(fifo_buffer_t* buffer, bool blocking);
void** fifoFlush(fifo_buffer_t* buffer);
int fifoUpdateOccupancy(fifo_buffer_t* buffer);
void fifoPrint(fifo_buffer_t* buffer);
fifo_buffer_t* fifoBufferInit(int max_buffer_size);
fifo_node_t* fifoNodeCreate(void* data, int priority);
void** fifoBufferClose(fifo_buffer_t*);
void* fifoNodeDestroy(fifo_node_t*);