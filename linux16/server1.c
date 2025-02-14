#include <func.h>
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
    struct sockaddr_in clientAddr;
    socklen_t socklen = sizeof(clientAddr);
    int netfd = accept(sockfd,(struct sockaddr*) &clientAddr, (socklen_t*)&socklen);

    int epfd = epoll_create(1);//create epoll file
    struct epoll_event ev, events[2] ;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    ev.events = EPOLLIN;
    ev.data.fd = netfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, netfd, &ev);
    char buff[4096];
    while(1){
        int readyNum = epoll_wait(epfd, events, 2, -1);
        for (int i=0; i<readyNum; ++i){
            if(events[i].data.fd == STDIN_FILENO){
                bzero(buff, 0);
                read(STDIN_FILENO, buff, sizeof(buff));
                send(netfd, buff, strlen(buff), 0);
            }else if(events[i].data.fd == netfd){
                bzero(buff, 0);
                recv(netfd, buff, sizeof(buff), 0);
                printf("recv: %s", buff);
            }
        }
    }
    return 0;
}

