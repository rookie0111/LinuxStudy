#include "head.h"
int main(int argc, char* argv[]){
    workerdata_t workerArr[3];
    
    makeWorker(3, workerArr);
    for (int i=0; i<3; ++i)
    {
        printf("pid = %d\n", workerArr[i].pid);
    }
    while(1){
        sleep(1);
    }
    return 0;
}
