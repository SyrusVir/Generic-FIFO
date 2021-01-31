/**
 * Author: Jude Alnas
 * Date: Jan 30 2021
 * Description: A generic, thread-safe FIFO buffer. Buffers data pointers. Includes priority functionality
 * **/
#include <fifo.h>

int fifoPush(fifo_buffer_t* buffer, void* data, int priority, bool blocking) {
    /**Push data into buffer. If blocking == true, this function will wait until
     * the buffer is available and non-full. 0 is returned if push is successful.
     * If priority < 0, data will be appended at the end of FIFO and will be the 
     * next node retrieved by fifoPUll. Otherwise, the node is inserted behind the 
     * first node of equal or greater priority.  **/

    int lock_status;

    //First acquire mutex lock///////////////////////////
    if (blocking) {
        lock_status = pthread_mutex_lock(&buffer->lock);
    } //if blocking desired, wait until mutex obtained
    else {
        lock_status = pthread_mutex_trylock(&buffer->lock);
    } //else try non-blocking lock
    ////////////////////////////////////////////////////////

    //Push node into buffer if mutex acquired ///////////////////////
    if (lock_status == 0) {
        if(buffer->buffer_occupancy >= buffer->max_buffer_size) {
            if (blocking) {
                pthread_cond_wait(&buffer->cond_nonfull, &buffer->lock); 
            } //if blocking set, wait until nonfull signal is emitted
            else {
                return -1;
            } //otw return immediately with -1;
        } //If buffer full, wait or return

        //This point reached only if mutex is obtained and nonfull signal has been emitted
        fifo_node_t* new_node = fifoNodeCreate(data, priority); //initialize new buffer node

        if (buffer->head == NULL) {
            new_node->prev = NULL;
            new_node->next = NULL;
            buffer->head = new_node;
            buffer->tail = new_node;
        } //If buffer empty, initialize appropriately
        else if (priority < 0) {
            new_node->prev = buffer->tail;
            new_node->next = NULL;
            buffer->tail = new_node;
        } //Negative priorities are emergencies. Append to tail
        else {
            //loop through buffer until a node of equal or higher priority is found. 
            //p == head if buffer empty or existing nodes are higher priority
            //p == NULL if buffer empty
            fifo_node_t* p;
            for (p = buffer->head; p != NULL;p = p->next) {
                if (p->priority >= priority) break;
            }

            new_node->next = p;
            if (p == buffer->head) { 
                new_node->prev = NULL;
                buffer->head->prev = new_node;
                buffer->head = new_node;
            } //p == head when new node has priority lower than existing nodes
            
            else if (p == NULL) {
                new_node->prev = buffer->tail;
                new_node->next = NULL;
                buffer->tail = new_node;
            } //Append to tail  when new node priority higher than existing nodes
            
            else {
                new_node->prev = p->prev;
                p->prev->next = new_node;
                p->prev = new_node;            
            } //Insert appropriately if p != NULL && p != head
        } //For non-negative priorities, find node of equal or greater priority. Insert new node before.

        buffer->buffer_occupancy++;
        pthread_cond_signal(&buffer->cond_nonempty);
        pthread_mutex_unlock(&buffer->lock);
    } // continue if lock successfully obtained
    ///////////////////////////////////////////////////////////

    return lock_status;
}   

void* fifoPull(fifo_buffer_t* buffer, bool blocking) {
    /**Returns the next data pointer in the FIFO pointed to by buffer.
     * If blocking == true, this call will block until the buffer is
     * available and non-empty. A non-null pointer is returned if the 
     * pull was successful.   **/

    int lock_status;
    void* out = NULL; 
    //First acquire mutex lock///////////////////////////
    if (blocking) {
        lock_status = pthread_mutex_lock(&buffer->lock);
    } //if blocking desired, wait until mutex obtained
    else {
        lock_status = pthread_mutex_trylock(&buffer->lock);
    } //else try non-blocking lock
    ////////////////////////////////////////////////////////

    //Pull from buffer if lock acquired /////////////////////////////
    if (lock_status == 0){
        if(buffer->buffer_occupancy <= 0) {
            if (blocking) {
                pthread_cond_wait(&buffer->cond_nonempty, &buffer->lock); 
            } //if blocking set, wait until nonfull signal is emitted
            else {
                return NULL;
            } //otw return immediately with NULL;
        } //If buffer full, wait or return

        //This point is reached if the buffer is available and nonempty
        fifo_node_t* rec = buffer->tail;    //save buffer tail
        buffer->tail = buffer->tail->prev;  //move buffer tail back
        if (buffer->tail == NULL) {
            buffer->head = NULL;
        } //If tail is NULL, then buffer is empty. Update head
        out = fifoNodeDestroy(rec);         //deallocate memory and return data pointer

        buffer->buffer_occupancy--;
        pthread_cond_signal(&buffer->cond_nonfull);
        pthread_mutex_unlock(&buffer->lock);
    }
    /////////////////////////////////////////////////////////////////

    return out;
}

void** fifoFlush(fifo_buffer_t* buffer) {
    /**Empties the buffer, returning a NULL-terminated 
     * array of pointers pointing to the data contents **/
    
    void** out = (void*) calloc(buffer->buffer_occupancy + 1,sizeof(void*));
    fifo_node_t* p_tmp;
    fifo_node_t* p = buffer->head;
    int i = 0;
    while (p != NULL) {
        p_tmp = p; //store pointer to current node
        p = p->next; //get pointer to next node
        out[i] = fifoNodeDestroy(p_tmp); //destroy current node
        i++;
    }
    
    out[i] = NULL; //Append NULL termination
    
    //empty buffer configuration
    buffer->head = NULL;
    buffer->tail = NULL;

    return out;
}

int fifoUpdateOccupancy(fifo_buffer_t* buffer) {
    /**A more accurate means of determining buffer occupancy.
     * The buffer is iterated through its entirety, counting the entries.**/

    int cnt = 0;
    fifo_node_t* p;
    for (p = buffer->head; p != NULL; p = p->next) cnt++;
    
    buffer->buffer_occupancy = cnt;
    return cnt;
}

void fifoPrint(fifo_buffer_t* buffer) {
    fifo_node_t* p;
    int i = 0;
    
    for (p = buffer->head; p != NULL; p = p->next) {
        printf(" Node %d:  Address=%p  Priority=%d  Next=%p  Prev=%p\n",i,p,p->priority,p->next,p->prev);
        i++;
    }
}

fifo_buffer_t* fifoBufferInit(int max_buffer_size) {
    fifo_buffer_t *buffer = (fifo_buffer_t*) malloc(sizeof(fifo_buffer_t));
    
    pthread_mutex_init(&buffer->lock,NULL);
    pthread_cond_init(&buffer->cond_nonfull,NULL);
    pthread_cond_init(&buffer->cond_nonempty,NULL);
    buffer->max_buffer_size = max_buffer_size;
    buffer->buffer_occupancy = 0;
    buffer->head = NULL;
    buffer->tail = NULL;

    return buffer;
}

fifo_node_t* fifoNodeCreate(void* data, int priority) {
    /** Returns pointer to a FIFO buffer node in allocated memory.
     * Next and Prev are intialized to NULL. Pair with fifoNodeDestroy
     * to ensure memory is freed and contined data is preserved**/

    fifo_node_t *node_out = (fifo_node_t*) malloc(sizeof(fifo_node_t));
    node_out->priority = priority;
    node_out->data = data;
    node_out->next = NULL;
    node_out->prev = NULL;

    return node_out;
}

void** fifoBufferClose(fifo_buffer_t* buffer) {
    /**Closes all references to the buffer, returning the data 
     * conents remaining in a NULL-terminated array of pointers**/
    
    void* *out = fifoFlush(buffer);
    pthread_mutex_destroy(&buffer->lock);
    pthread_cond_destroy(&buffer->cond_nonempty); 
    pthread_cond_destroy(&buffer->cond_nonfull);
    free(buffer);

    return out; 
}

void* fifoNodeDestroy(fifo_node_t* node) {
    /** Returns data pointer contained in node and frees allocated memory **/
    void* out = node->data;
    free(node);
    return out;
}