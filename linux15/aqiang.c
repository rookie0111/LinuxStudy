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

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int netfd = accept(listenfd, (struct sockaddr*)& client_addr, &addr_len);
    if (netfd == -1){
        error(1, errno, "accept");
    }

    // set monitor set
    fd_set mainfds;
    FD_ZERO(&mainfds);
    FD_SET(STDIN_FILENO, &mainfds);
    int maxfds = STDIN_FILENO;
    FD_SET(netfd, &mainfds);
    if (netfd > maxfds){
        maxfds = netfd;
    } 
    // define send and recv buffer.
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    while(1){
        fd_set readfds = mainfds;// need monitor file number.
        int events = select(maxfds + 1, &readfds, NULL, NULL, NULL);

        switch(events){
            case -1: 
                error(1, errno, "select");
            case 0:
                printf("timeout\n");
            default:
                if (FD_ISSET(STDIN_FILENO, &readfds)){
                    fgets(sendline, MAXLINE, stdin);
                    send(netfd, sendline, strlen(sendline) + 1, 0);
                }
                if (FD_ISSET(netfd, &readfds)){
                    int nbytes = recv(netfd, recvline, MAXLINE, 0);
                    if (nbytes == 0){
                        goto end;
                    }else if(nbytes == -1){
                        error(1, errno, "recv error");
                    }

                    printf("azhen: %s", recvline);
                }
        }
    }

end:
    close(netfd);

    return 0;
}
