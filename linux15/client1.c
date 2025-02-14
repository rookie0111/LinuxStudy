#include <func.h>

int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "usage: ./client ip_address port");
    }

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);

    if (clientfd == -1){
        error(0, errno, "socket "); 
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET; // set protocal type ipv4
    serveraddr.sin_port = htons(atoi(argv[2])); // 小端转化为大端法存储
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]); // 点分十进制转换为uint32_t ip address
    int ret = connect(clientfd, (const struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (ret == -1){
        error(1, errno, "connect error");
    }

    ssize_t sret1 = send(clientfd, "Hello", 5, 0);
    printf("sret1 = %ld\n", sret1);
    char buff[4096];
    ssize_t sret2 = recv(clientfd, buff, sizeof(buff), 0);
    printf("sret2 = %ld, buff = %s\n", sret2, buff);
    return 0;
}
