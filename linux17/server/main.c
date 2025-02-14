#include "head.h"
int exitPipe[2];
void handle(int signum){
    printf("signum = %d\n", signum);
    write(exitPipe[1], "1", 1);// 主进程自用pipe,一端写入，另一端读就绪
}
int main(int argc, char* argv[]){
    // ./server ip port processNum.
    if (argc < 4){
        error(1, 1, "./server IP port processNum");
    }
    int workerNum = atoi(argv[3]);
    // 创建进程池
    workerdata_t* workerArr = (workerdata_t*)calloc(workerNum, sizeof(workerdata_t));
    // 初始化进程池
    makeWorker(workerNum, workerArr);
    printf("makeWorker is OK\n");
    // 注册信号
    pipe(exitPipe);
    signal(SIGUSR1, handle);

    int sockfd;
    // 初始化连接
    tcpInit(argv[1], argv[2], &sockfd);
    printf("tcpInit is OK\n");
    // init epoll
    int epfd = epoll_create(1);
    // sockfd 加入监听
    epollAdd(epfd, sockfd);
    epollAdd(epfd, exitPipe[0]);
    for(int i=0; i<workerNum; ++i){
        // 监听每个子进程的pipesockfd是否就绪
        epollAdd(epfd, workerArr[i].pipesockfd);
    }
    while(1){
        struct epoll_event readySet[MAX_EVENTS];// 接受就绪文件描述符
        int readyNum = epoll_wait(epfd, readySet, MAX_EVENTS, -1);
        for (int i=0; i<readyNum; ++i){
            if (readySet[i].data.fd == sockfd){
                int netfd = accept(sockfd, NULL, NULL);
                printf("one client is connected!!!\n");
                //顺序查找空闲进程i,将文件描述符发送给子进程
                for (int j=0; j<workerNum; ++j){
                    if(workerArr[j].status == FREE){
                        sendfd(workerArr[j].pipesockfd, netfd, 0);
                        workerArr[j].status = BUSY;
                        break;
                    }
                }
                close(netfd);// 没有找到就断开连接
            }else if(readySet[i].data.fd == exitPipe[0]){
                // 进程退出
                for(int j=0; j<workerNum; ++j){
                    //kill(workerArr[j].pid, SIGKILL);暴力杀死
                    sendfd(workerArr[i].pipesockfd, 0, 1);// 传1号信号
                    printf("kill one worker!\n");
                }
                for(int j=0; j<workerNum; ++j){
                    printf("j = %d\n", j);
                    wait(NULL);//回收子进程资源
                }
                printf("All workers are killed!\n");
                exit(0);
            }
            else{
                //子进程完任务,获取子进程
                int pipesockfd = readySet[i].data.fd;
                //找到这个子进程更改其状态为FREE
                for(int j=0; j<workerNum; ++j){
                    if(workerArr[j].pipesockfd == pipesockfd){
                        pid_t pid;
                        recv(pipesockfd, &pid, sizeof(pid), 0);
                        printf("%d worker, pid = %d\n", j, pid);
                        workerArr[j].status = FREE;
                        break;
                    }
                }
            }
        }
    }
    return 0;
}
