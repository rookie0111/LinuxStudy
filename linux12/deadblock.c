#include <func.h>

typedef struct{
    int id;
    char name[25];
    int balance;
    
    pthread_mutex_t mutex;
}Account;

pthread_t protection = PTHREAD_MUTEX_INITIALIZER;
Account acct1 = {1, "li", 1000, PTHREAD_MUTEX_INITIALIZER};
Account acct2 = {2, "wu", 100, PTHREAD_MUTEX_INITIALIZER};

int transfer(Account* acctA, Account* acctB, int money){
   
    // 破坏循环等待
    //if(acctA->id > acctB->id){
    //    pthread_mutex_lock(&acctA->mutex);
    //    sleep(1);
    //    pthread_mutex_lock(&acctB->mutex);
    //}else{
    //    pthread_mutex_lock(&acctB->mutex);
    //    sleep(1);
    //    pthread_mutex_lock(&acctA->mutex);
    //}

    // 不能抢占条件
//start:
//    pthread_mutex_lock(&acctA->mutex);
//    sleep(1);
//    int err = pthread_mutex_trylock(&acctB->mutex);
//    if (err){
//        pthread_mutex_unlock(&acctA->mutex);
//        int second = rand() % 10;
//        sleep(second); // 获取失败就睡眠，错开获取锁增加获取成功概率
//        goto start;
//    }

    // 持有并等待,与使用全局锁的区别在于枷锁位置仅限于获取锁部分，对于执行整个过程不加锁耗时较小
    pthread_mutex_lock(&protection);
    pthread_mutex_lock(&acctA->mutex);
    sleep(1);
    pthread_mutex_lock(&acctB->mutex);
    pthread_mutex_unlock(&protection);


    acctA->balance -= money;
    acctB->balance += money;

    pthread_mutex_unlock(&acctA->mutex);
    pthread_mutex_unlock(&acctB->mutex);

    return money;
}

void* start_routine1(void* args){
    int money = (int) args;
    int ret = transfer(&acct1, &acct2, money);
    printf("%s -> %s %d \n", acct1.name, acct2.name, ret);
    return NULL;
}

void* start_routine2(void* args){
    int money = (int) args;
    int ret = transfer(&acct2, &acct1, money);
    printf("%s -> %s %d \n", acct2.name, acct1.name, ret);
    return NULL;
}

int main(int argc, char* argv[]){
   
    pthread_t tid1, tid2;
    int err = pthread_create(&tid1, NULL, start_routine1, (void*)900);
    if (err){
        error(1, errno, "tid1 error");
    }

    err = pthread_create(&tid2, NULL, start_routine2, (void*)100);
    if (err){
        error(1, errno, "tid2 error");
    }

    //wait child thread end
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("%s account is : %d\n", acct1.name, acct1.balance);
    printf("%s account is : %d\n", acct2.name, acct2.balance);
    return 0;
}

