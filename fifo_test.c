#include <fifo.h>
#include <time.h>
#include <unistd.h>

void* fifoMain(void* arg_buffer) {
    fifo_buffer_t* buffer = (fifo_buffer_t*) arg_buffer;
    pthread_t self_tid = pthread_self();
    while(1) {
        int* rec = fifoPull(buffer,true);
        usleep(10000);
        if (*rec < 0) break;
        else printf("Thread %ld: Received %d\n", self_tid,*rec);
    }
}

int main() {
    fifo_buffer_t* buffer = fifoBufferInit(50);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, &fifoMain, (void*)buffer);
    srand(time(NULL));

    for (int i = 0; i < 500; i++) {
        int* p = (int*)malloc(sizeof(int));
        int priority;
        *p = i;
    
        int min = -20;
        int max = 20;
        priority = min + rand()/(RAND_MAX/(max-(min)+1)+1);

        printf("(%d,%d)\n",priority,*p);

        fifoPush(buffer,(void*)p,priority,true);
        //printf("PUSHED\n");
    }
    
    int i = -1;
    fifoPush(buffer, (void*)&i,0,true);
    pthread_join(thread_id,NULL);
    
    printf("After join\n");
    int** leftover = (int**)fifoBufferClose(buffer);
    for(;*leftover != NULL; leftover++) {
        printf("%d\n", **leftover);
        free(*leftover);
    }
    return 0;
}