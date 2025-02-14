#ifndef __BLOCKQ_H
#define __BLOCKQ_H

#include <func.h>
#define N 1024

typedef int E;

typedef struct{
    E elements[N];
    int front;
    int rear;
    int size;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;

}BlockQ;

BlockQ* blockq_create(void);
void blockq_destroy(BlockQ* q);

void blockq_push(BlockQ* q, E val);
E blockq_pop(BlockQ* q);
E blockq_peek(BlockQ* q);
bool blockq_empty(BlockQ* q);
bool blockq_full(BlockQ* q);

#endif
