#include <func.h>
#define MAXLINE 256
int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "usage: ./client ip_address port");
    }

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);

    if (clientfd == -1){
        error(0, errno, "socket "); 
    }
    // build tcp connect.
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET; // set protocal type ipv4
    serveraddr.sin_port = htons(atoi(argv[2])); // 小端转化为大端法存储
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]); // 点分十进制转换为uint32_t ip address
    int ret = connect(clientfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == -1){
        error(1, errno, "connect error");
    }
    // set monitor set
    fd_set mainfds;
    FD_ZERO(&mainfds);
    FD_SET(STDIN_FILENO, &mainfds);
    int maxfds = STDIN_FILENO;
    FD_SET(clientfd, &mainfds);
    if (clientfd > maxfds){
        maxfds = clientfd;
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
                    send(clientfd, sendline, strlen(sendline) + 1, 0);
                }
                if (FD_ISSET(clientfd, &readfds)){
                    int nbytes = recv(clientfd, recvline, MAXLINE, 0);
                    if (nbytes == 0){
                        goto end;
                    }else if(nbytes == -1){
                        error(1, errno, "recv error");
                    }

                    printf("aqiang: %s", recvline);
                }
        }
    }

end:
    close(clientfd);

    return 0;
}
