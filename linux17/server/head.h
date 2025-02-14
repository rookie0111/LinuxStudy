#include <func.h>

#define MAX_EVENTS 1024

enum{
    FREE, 
    BUSY
};

//define process info
typedef struct workerdata_s{
    pid_t pid;
    int status;
    int pipesockfd;// 父子进程通信fd
}workerdata_t;

int makeWorker(int workerNum, workerdata_t* workerArr);
int tcpInit(const char* ip, const char* port, int* psockfd);
int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);
int sendfd(int sockfd, int fdtosend, int exitFlag);
int recvfd(int sockfd, int *pfdtorecv, int *pexitFlag);
int workLoop(int sockfd);
int transFile(int netfd);
