#include "head.h"
int tcpInit(const char* ip, const char* port, int* psockfd){
    *psockfd = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    int ret = setsockopt(*psockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(port));
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    ret = bind(*psockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if (ret == -1){
        error(1, errno, "bind");
    }
    listen(*psockfd, 50);
    return 0;
} 
