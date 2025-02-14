#include <func.h>
#include "BlockQ.h"

typedef struct{
    pthread_t* threads;
    int nums;
    BlockQ* q;
}ThreadPool;

void* start_routine(void* arg){
    ThreadPool* pool = (ThreadPool*)arg;
    pthread_t tid = pthread_self();

    for (;;){
        E task_id = blockq_pop(pool->q);
    if (task_id == -1){
            pthread_exit(NULL);
        }
        // 模拟任务执行
        printf("%lx task %d start\n", tid, task_id);
        sleep(1);
        printf("%lx task %d end\n", tid, task_id);
    }
    return NULL;
}

ThreadPool* threadpool_create(int n){
    ThreadPool* pool = (ThreadPool*) malloc(sizeof(ThreadPool));
    if (!pool){
        error(1, 1, "malloc");
    }

    pool->threads = (pthread_t*) malloc(n * sizeof(pthread_t));
    pool->nums = n;
    pool->q = blockq_create();
    for (int i=0; i<n; i++){
        pthread_create(pool->threads + i, NULL, start_routine, (void*)pool);
    }
    return pool;
}

void threadpool_destroy(ThreadPool* pool){
    blockq_destroy(pool->q);
    free(pool->threads);
    free(pool);
}

int main(int argc, char* argv[]){
    // 创建线程池
    ThreadPool* pool = threadpool_create(8);
    // 主线程生产
    for (int i=0; i<100; i++){
        blockq_push(pool->q, i+1);
    }

    for (int i=0; i<pool->nums; i++){
        blockq_push(pool->q, -1);
    }
    sleep(3);
    // 退出线程池,向阻塞队列加入结束任务

    for  (int i=0; i<pool->nums; i++){
       pthread_join(pool->threads[i], NULL);
    }
    // 销毁线程池
    threadpool_destroy(pool);


    return 0;
}
