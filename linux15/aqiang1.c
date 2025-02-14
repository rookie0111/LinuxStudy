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

    // set monitor set
    fd_set monitorSet;
    fd_set readySet;
    int netfd = -1;
    FD_ZERO(&monitorSet);
    FD_ZERO(&readySet);
    FD_SET(listenfd, &monitorSet);
    printf("Already Connecting!\n");
    int maxfds = STDIN_FILENO;
    if (listenfd > maxfds){
        maxfds = listenfd;
    }
    // define send and recv buffer.
    char sendline[MAXLINE];
    char recvline[MAXLINE];


    while(1){
        readySet = monitorSet;// need monitor file number.
        int events = select(maxfds + 1, &readySet, NULL, NULL, NULL);

        switch(events){
            case -1: 
                error(1, errno, "select");
            case 0:
                printf("timeout\n");
            default:
                if (FD_ISSET(listenfd, &readySet)){
                        netfd = accept(listenfd, NULL, NULL);
                        
                        if (netfd == -1){
                            error(1, errno, "accept");
                        }
                        if (netfd > maxfds){
                            maxfds = netfd;
                        }

                        FD_CLR(listenfd, &monitorSet);
                        FD_SET(STDIN_FILENO, &monitorSet);
                        FD_SET(netfd, &monitorSet);

                    }

               if (FD_ISSET(netfd, &readySet)){
                    ssize_t nbytes = recv(netfd, recvline, MAXLINE, 0);
                    printf("nbytes: %ld\n", nbytes);
                    if (nbytes == 0){
                        FD_SET(listenfd, &monitorSet);
                        FD_CLR(STDIN_FILENO, &monitorSet);
                        FD_CLR(netfd, &monitorSet);
                        close(netfd);
                        netfd = -1;
                        if (listenfd > maxfds){
                            maxfds = listenfd;
                        }
                        printf("azhen already disconnect! \n");
                        continue;

                    }else  if(nbytes == -1){
                        error(1, errno, "recv error");
                    }

                    printf("azhen: %s", recvline);
                }

                if (FD_ISSET(STDIN_FILENO, &readySet)){
                    bzero(sendline, sizeof(sendline));
                   // fgets(sendline, MAXLINE, stdin);
                   // ssize_t sret = send(netfd, sendline, strlen(sendline), 0);
                    ssize_t sret = read(STDIN_FILENO, sendline, sizeof(sendline));
                    printf("sret = %ld\n", sret);// 换行符也会存入，当
                    if (sret == 0){
                        
                        FD_SET(listenfd, &monitorSet);
                        FD_CLR(STDIN_FILENO, &monitorSet);
                        FD_CLR(netfd, &monitorSet);
                        close(netfd);
                        netfd = -1;
                        if (listenfd > maxfds){
                            maxfds = listenfd;
                        }
                        printf("I already disconnect! \n");
                        continue;
                    }
                   send(netfd, sendline, strlen(sendline)+1, 0);
                }

        }
    }

    return 0;
}
