#include "fifo.h"

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* recThread(void* arg_fifo_buff)
{
    fifo_buffer_t* fifo_buff = (fifo_buffer_t*) arg_fifo_buff;

    int recv_int;
    do
    {
        int* recv = (int*)fifoPull(fifo_buff, true);
        recv_int = *recv;
        free(recv);
        printf("recv_int=%d\n", recv_int);
    } while (recv_int > -1);
    
    return;
}

int main()
{
    pthread_t rec_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    fifo_buffer_t* buff = fifoBufferInit(10);

    pthread_create(&rec_thread, &attr, &recThread, buff);

    for (int i = 0; i < 50; i++)
    {
        int* data = (int*)malloc(sizeof(int));
        *data = i;
        fifoPush(buff, data, 0, false);
        printf("pushed %d\n", *data);
    }
    int* data = (int*)malloc(sizeof(int));
    *data = -1;
    fifoPush(buff, data, 0, true);
    printf("pushed %d\n", *data);
    pthread_join(rec_thread, NULL);
    return 0;
}