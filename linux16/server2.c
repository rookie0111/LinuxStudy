#include <func.h>
#define MAXNUM 1024
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
    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    ret = listen(sockfd, 50);
    int netfd = -1;

    int epfd = epoll_create(1);//create epoll file
    struct epoll_event ev, events[MAXNUM] ;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    char buff[4096];
    while(1){
        int readyNum = epoll_wait(epfd, events, MAXNUM, -1);
        for (int i=0; i<readyNum; ++i){
            if(events[i].data.fd == sockfd){
                netfd = accept(sockfd, NULL, NULL);
                printf("netfd = %d \n", netfd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                ev.events = EPOLLIN;
                ev.data.fd = STDIN_FILENO;
                epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
                ev.events = EPOLLIN;
                ev.data.fd = netfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, netfd, &ev);
            }
            
            else if (netfd != -1 && events[i].data.fd == netfd){
                bzero(buff, 0);
                ret = recv(netfd, buff, sizeof(buff), 0);
                if (ret == 0){
                    printf("client %d will be closed.\n", netfd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, STDIN_FILENO, NULL);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, netfd, NULL);
                    ev.events = EPOLLIN;
                    ev.data.fd = sockfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
                    close(netfd);
                    netfd = -1;
                    break;
                }
                printf("recv: %s", buff);
            }

            else if (netfd != -1 && events[i].data.fd == STDIN_FILENO){
                bzero(buff, 0);
                ret = read(STDIN_FILENO, buff, sizeof(buff));
                if (ret == 0){
                    printf("server will be closed.\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, STDIN_FILENO, NULL);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, netfd, NULL);
                    ev.events = EPOLLIN;
                    ev.data.fd = sockfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
                    close(netfd);
                    netfd = -1;
                    break;
                }
                send(netfd, buff, strlen(buff)+1, 0);
//                printf("recv: %s", buff);
            }

        }
    }
    return 0;
}

