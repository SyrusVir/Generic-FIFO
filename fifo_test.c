#include <fifo.h>
#include <time.h>

int fifoMain(void* arg_buffer) {
    fifo_buffer_t* buffer = (fifo_buffer_t*) arg_buffer;
    pthread_t self_tid = pthread_self();
    while(1) {
        int* rec = fifoPull(buffer,true);
        
        if (*rec < 0) break;
        else printf("Thread %ld: Received %d\n", self_tid,*rec);

    }
}

int main() {
    fifo_buffer_t* buffer = fifoBufferInit(20);
    pthread_t thread_id;
   // pthread_create(&thread_id, NULL, &fifoMain, (void*)buffer);
    srand(time(NULL));

    for (int i = 0; i < 10; i++) {
        int* p = (int*)malloc(sizeof(int));
        *p = i;

        int min = -5;
        int max = 10;
        int priority = min + rand()/(RAND_MAX/(max-(min)+1)+1);

        fifoPush(buffer,(void*)p,priority,true);
    }

    fifoPrint(buffer);
    /* int i = -1;
    fifoPush(buffer, (void*)&i,0,true);
    pthread_join(thread_id,NULL); */
    
    int** leftover = (int**)fifoBufferClose(buffer);
    for(;*leftover != NULL; leftover++) {
        printf("%d\n", **leftover);
        free(*leftover);
    }
    return 0;
}