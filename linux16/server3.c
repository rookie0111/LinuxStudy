#include <func.h>
#define MAXNUM 1024
typedef struct Conn_s{
    int isAlive;
    int netfd;
    time_t lastActive;
}Conn_t;

int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "./server1 IP port");
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    int reuse = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret == -1){
        error(1, errno, "setsockopt error");
    }

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));    
    if (ret == -1){
        error(1, errno, "bind error");
    }
    ret = listen(sockfd, 50);
    if (ret == -1){
        error(1, errno, "listen error");
    }
    int netfd = -1;

    int epfd = epoll_create(1);//create epoll file
    struct epoll_event ev, readySet[MAXNUM] ;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    Conn_t conn[MAXNUM];
    for (int i=0; i<MAXNUM; ++i){
        conn[i].isAlive = 0;
        //conn[i].netfd = -1;
    }

    int fdtoidx[MAXNUM];// fdtoidx[netfd] = idx  conn[idx]
    for (int i=0; i<MAXNUM; ++i){
        fdtoidx[i] = -1;
    }
    char buff[4096];
    int curidx = 0;
    time_t now;
    while(1){// every 1 sencond is ready.
        int readyNum = epoll_wait(epfd, readySet, MAXNUM, 1000);
        now = time(NULL);
        printf("now = %s\n", ctime(&now));
        for (int i=0; i<readyNum; ++i){
            if(readySet[i].data.fd == sockfd){
                netfd = accept(sockfd, NULL, NULL);
                // update index and connetion status.
                conn[curidx].isAlive = 1;
                conn[curidx].netfd = netfd;
                conn[curidx].lastActive = time(NULL);
                fdtoidx[netfd] = curidx;
                // monitor netfd and 
                printf("netfd = %d \n", netfd);
                ev.events = EPOLLIN;
                ev.data.fd = netfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, netfd, &ev);
                ++curidx;
            }else{
                int netfd = readySet[i].data.fd;
                bzero(buff, sizeof(buff));
                ssize_t sret = recv(netfd, buff, sizeof(buff), 0);
                // sret is zero represent client disconnected.
                if (sret == 0){
                    printf("%d client is closed\n", netfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, netfd, NULL);
                    int idx = fdtoidx[netfd];
                    conn[idx].isAlive = 0;
                    fdtoidx[netfd] = -1;
                    close(netfd);
                    continue;
                }
                int idx = fdtoidx[netfd];
                conn[idx].lastActive = time(NULL);
                // transmit recv message to others on line clients.
                for (int j=0; j<curidx; ++j){
                    if (conn[j].isAlive == 1 && conn[j].netfd != netfd){
                       send(conn[j].netfd, buff, strlen(buff), 0); 
                    }
                }
        
            }
        }
        for (int i=0; i<curidx; ++i){
            if (now - conn[i].lastActive > 10){
                printf("netfd = %d timeout\n", conn[i].netfd); 
                close(conn[i].netfd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, conn[i].netfd, NULL);
                conn[i].isAlive = 0;
                fdtoidx[conn[i].netfd] = -1;
            } 
        }
    }
    return 0;
}

