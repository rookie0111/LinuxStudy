#include "head.h"
int makeWorker(int workerNum, workerdata_t * workerArr){
    for (int i=0; i<workerNum; ++i){
        pid_t pid = fork();
        if (pid == -1){
            error(1, errno, "makeWorker pid error");
        }else if (pid == 0){
            while(1){
                sleep(1);
            }//child process do not make new process.
        }
        workerArr[i].pid = pid;
        workerArr[i].status = FREE;
       // printf("new process created\n pid = %d, status = %d",
         //      workerArr[i].pid, workerArr[i].status);
    } 
    return 0;
}
