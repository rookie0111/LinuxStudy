#include "BlockQ.h"

BlockQ* blockq_create(){
    BlockQ* q = (BlockQ*) calloc(1, sizeof(BlockQ));

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    return q;
}

void blockq_destroy(BlockQ* q){
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);

    free(q);
}

void blockq_push(BlockQ* q, E val){
    pthread_mutex_lock(&q->mutex);
    while(q->size == N){
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    q->elements[q->rear] = val;
    q->rear = (q->rear + 1) % N;
    q->size++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

E blockq_pop(BlockQ* q){
    pthread_mutex_lock(&q->mutex);
    while(q->size == 0){ // 如果队列为空，就阻塞，直到不空
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    E ret = q->elements[q->front];
    q->front = (q->front + 1) % N;
    q->size--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

E blockq_peek(BlockQ* q){
    pthread_mutex_lock(&q->mutex);
    while(q->size == 0){
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    E ret = q->elements[q->front];
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

bool blockq_full(BlockQ* q){
    pthread_mutex_lock(&q->mutex);
    int size = q->size;
    pthread_mutex_unlock(&q->mutex);
    return size == N;
}


bool blockq_empty(BlockQ* q){
    pthread_mutex_lock(&q->mutex);
    int size = q->size;
    pthread_mutex_unlock(&q->mutex);
    return size == 0;
}
