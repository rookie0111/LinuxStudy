#include <func.h>
#define MAXLINE 256
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
    int netfd[1024];
    for (int i=0; i<1024; i++){
        netfd[i] = -1;
    }
    int curidx = 0; // the index of next time needed to join netfd. 
    int fdToidx[1024];// relect netfd to index, used to find location int netfd.
    for (int i=0; i<1024; i++){
        fdToidx[i] = -1;
    }

    char buff[4096];
    while(1){
        memcpy(&readySet, &monitorSet, sizeof(monitorSet));
        select(1024, &readySet, NULL, NULL, NULL);
        if (FD_ISSET(listenfd,&readySet)){//whether  monitor set is ready.
            netfd[curidx] = accept(listenfd, NULL, NULL);
            printf("i = %d, netfd = %d\n", curidx, netfd[curidx]);
            fdToidx[netfd[curidx]] = curidx;
            FD_SET(netfd[curidx], &monitorSet);
            curidx++;
        }

        //whether one of netfd is ready.
        for (int i=0; i<curidx; i++){
            if (netfd[i] != -1 && FD_ISSET(netfd[i], &readySet)){
                // -1 represent is connecting, and now is already.
                bzero(buff, sizeof(buff));
                ssize_t sret = recv(netfd[i], buff, sizeof(buff), 0);
                if (sret == 0){
                    FD_CLR(netfd[i], &monitorSet);
                    close(netfd[i]);
                    fdToidx[netfd[i]] = -1;
                    netfd[i] = -1;
                    continue;
                }
                for(int j=0; j<curidx; j++){// send other clients message
                    if (netfd[j] != -1 && j != i){// not ifself and is connecting
                        send(netfd[j], buff, strlen(buff), 0);
                    }
                }
            }
        }

    }
    return 0;
}
