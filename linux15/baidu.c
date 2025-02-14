#include <func.h>
int main(int argc, char* argv[]){
    const char query[] = "GET / HTTP/1.0\r\n"// GET: quest type, / :root directroy, protocal type.
                        "Host: www.baidu.com\r\n" // quest hostname
                        "\r\n"; // quest ending

    const char hostname[] = "www.baidu.com";

    struct hostent *entry = gethostbyname(hostname);

    if (entry == NULL){
        error(1, errno, "gethostbyname");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;// ipv4
    addr.sin_port = htons(80);  // default port
    memcpy(&addr.sin_addr, entry->h_addr_list[0], sizeof(addr.sin_addr));
    
    printf("domain: %d, type: %d, ip: %s", 
           addr.sin_family, entry->h_addrtype, inet_ntoa(addr.sin_addr));
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        error(1, errno, "socket");
    }
    int sret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (sret == -1){
        error(1, errno, "connect");
    }
    send(sockfd, query, strlen(query), 0);
    char buff[4096];
    while(1){
        sret = recv(sockfd, buff, sizeof(buff), 0);
        if (sret == 0){
            break;
        }

        printf("%s", buff);
        bzero(buff, sizeof(buff));
    }
    return 0;
}
