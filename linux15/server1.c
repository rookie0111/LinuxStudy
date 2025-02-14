#include <func.h>

int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "usage: ./server ip_address port");
    }

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd == -1){
        error(0, errno, "socket "); 
    }

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET; // set protocal type ipv4
    serveraddr.sin_port = htons(atoi(argv[2])); // 小端转化为大端法存储
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]); // 点分十进制转换为uint32_t ip address

    // bind IP and port
    int ret = bind(listenfd, (const struct sockaddr*)& serveraddr, sizeof(serveraddr));
    if (ret == -1){
        error(1, errno, "bind error");
    }

    ret = listen(listenfd, 10);

    if (ret == -1){
        error(1, errno, "listen error");
    }


    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int netfd = accept(listenfd, (struct sockaddr*)&client_addr, &addr_len);
    if (netfd == -1){
        error(1, errno, "accpet");
    }
    printf("client ip address: %s, port: %d\n",
           inet_ntoa(client_addr.sin_addr), 
           ntohs(client_addr.sin_port));

    char buff[4096];
    //ssize_t sret1 = recv(netfd, buff, sizeof(buff), 0);// equal pipe, can use read
    ssize_t sret1 = read(netfd, buff, sizeof(buff));
    printf("sret1 = %ld,buff =  %s\n", sret1, buff);
    //ssize_t sret2 = send(netfd, "what is your name?", 18, 0);//also use write.
    ssize_t sret2 = write(netfd, "what is your name?", 18);
    printf("sret2: %ld\n", sret2);
    return 0;
}
