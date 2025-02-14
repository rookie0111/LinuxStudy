#include <func.h>
#define MAXLINE 256
typedef struct Conn_s{
    int netfd;
    int isConnected;
    time_t lastActive;
}Conn_t;
int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "usage: ./server ip_address port");
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd == -1){
        error(0, errno, "socket "); 
    }
    // build tcp connect.
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET; // set protocal type ipv4
    serveraddr.sin_port = htons(atoi(argv[2])); // 小端转化为大端法存储
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]); // 点分十进制转换为uint32_t ip address
   
    int reuse = 1;// allow reuse
    int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (ret == -1){
        error(1, errno, "setsockopt");
    }

    ret = bind(listenfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == -1){
        error(1, errno, "bind error");
    }

    ret = listen(listenfd, 10);
    if (ret == -1){
        error(1, errno, "listen error");
    }

    fd_set monitorSet;
    fd_set readySet;
    FD_ZERO(&monitorSet);
    FD_SET(listenfd, &monitorSet);

    // use struct save all netfd.
    Conn_t client[1024];
    for (int i=0; i<1024; i++){
        client[i].isConnected = 0;
        client[i].lastActive = 0;
        client[i].netfd = -1;
    }
    int curidx = 0; // the index of next time needed to join netfd. 
    int fdToidx[1024];// relect netfd to index, used to find location int netfd.
    for (int i=0; i<1024; i++){
        fdToidx[i] = -1;
    }
    
    time_t now; // every time connect ready update.
    char buff[4096];
    while(1){
        memcpy(&readySet, &monitorSet, sizeof(monitorSet));
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        select(1024, &readySet, NULL, NULL, &timeout);
        now = time(NULL);
        printf("now = %s\n", ctime(&now));
        if (FD_ISSET(listenfd,&readySet)){//whether  monitor set is ready.
            client[curidx].netfd = accept(listenfd, NULL, NULL);
            client[curidx].isConnected = 1;
            client[curidx].lastActive = time(NULL);
            printf("i = %d, netfd = %d, now = %s\n",
                   curidx, client[curidx].netfd, ctime(&client[curidx].lastActive));
            fdToidx[client[curidx].netfd] = curidx;
            FD_SET(client[curidx].netfd, &monitorSet);
            curidx++;
        }

        //whether one of netfd is ready.
        for (int i=0; i<curidx; i++){
            if (client[i].isConnected == 1 && FD_ISSET(client[i].netfd, &readySet)){
                // -1 represent is connecting, and now is already.
                bzero(buff, sizeof(buff));
                ssize_t sret = recv(client[i].netfd, buff, sizeof(buff), 0);
                if (sret == 0){
                    FD_CLR(client[i].netfd, &monitorSet);
                    close(client[i].netfd);
                    fdToidx[client[i].netfd] = -1;
                    client[i].isConnected = 0;
                    printf("i = %d, netfd = %d diconnect\n", i, client[i].netfd);
                    continue;
                }

                client[i].lastActive = time(NULL);
                
                 printf("i = %d, netfd = %d, now = %s\n",
                       i, client[i].netfd, ctime(&client[i].lastActive));
                for(int j=0; j<curidx; j++){// send other clients message
                    if (client[j].isConnected == 1 && j != i){// not ifself and is connecting
                        send(client[j].netfd, buff, strlen(buff), 0);
                    }
                }
            }
        }

        //timeout check
        for (int i=0; i<curidx; i++){
            if (client[i].isConnected == 1 && now - client[i].lastActive > 5){
                // connecting and timeout will be disconnected.
                FD_CLR(client[i].netfd, &monitorSet);
                close(client[i].netfd);
                fdToidx[client[i].netfd] = -1;
                client[i].isConnected = 0;
                printf("i = %d, netfd = %d, timeout disconnected\n", i, client[i].netfd);
            }
        }

    }
    return 0;
}
