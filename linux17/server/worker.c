#include "head.h"
int makeWorker(int workerNum, workerdata_t * workerArr){
    for (int i=0; i<workerNum; ++i){
        int pipefd[2];// 接受文件描述符，从父进程中选取连个空闲文件描述符
        socketpair(AF_LOCAL, SOCK_STREAM, 0, pipefd);
        pid_t pid = fork();
        if (pid == -1){
            error(1, errno, "makeWorker pid error");
        }else if (pid == 0){
            close(pipefd[1]);
            workLoop(pipefd[0]);
            //child process do not make new process.
        }
        close(pipefd[0]);
        workerArr[i].pid = pid;
        workerArr[i].status = FREE;
        workerArr[i].pipesockfd = pipefd[1];
        printf("i = %d, pid = %d, pipefd = %d\n", i, pid, pipefd[1]);
    } 
    return 0;
}

int workLoop(int sockfd){
    while(1){
        int netfd;
        int exitFlag;
        recvfd(sockfd, &netfd, &exitFlag);// 使用子进程的一个空闲文件描述符指向主进程传送的fd.
        if(exitFlag == 1){
            printf("I am going to exit!\n");
            exit(0);
        }
        printf("begin woker!\n");
        //sleep(3);
        transFile(netfd);
        printf("woker over!\n");
        //任务完成 向master发送pid
        pid_t pid = getpid();
        send(sockfd, &pid, sizeof(pid), 0);
    }
}
