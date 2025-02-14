#include <func.h>
 // proctocal 1.0
 //int recvFile(int sockfd){
 //    char filename[4096] = {0};
 //    recv(sockfd, filename, sizeof(filename), 0);
 //    int fd = open(filename, O_CREAT|O_RDWR);
 //    char buf[1000] = {0};
 //    ssize_t sret = recv(fd, buf, sizeof(buf), 0);
 //    write(fd, buf, sret);
 //    return 0;
 //}
// 2.0 small file
typedef struct train_s{
    int length;// 长度标识
    char data[1000]; // 内容长度
}train_t;
int recvn(int sockfd, void* buf, long total){
    long cursize = 0;
    char* p = (char*) buf;// 方便进行统计字符，传输的是二进制数据
    while(cursize < total){
        ssize_t ret = recv(sockfd, p+cursize, total-cursize, 0);
        // 直接流失传输数据不会出现半包问题，不需要使用MSG_WAITALL
        cursize += ret;
    }
}
//int recvFile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recv(sockfd, &train.length, sizeof(train.length), 0);
//    recv(sockfd, train.data, train.length, 0);
//    // 使用memcpy不需要额外添加\0结束符，如果在前面使用过train会出现读取错误
//    memcpy(filename, train.data, train.length);
//    
//    int fd = open(train.data, O_CREAT|O_RDWR, 0666);
//    recv(sockfd, &train.length, sizeof(train.length), 0);
//    recv(sockfd, train.data, train.length, 0);
//    write(fd, train.data, train.length);
//    close(fd);
//    return 0;
//}

//3.0 big file
//int recvFile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
//    recv(sockfd, train.data, train.length, MSG_WAITALL);
//    // 使用memcpy不需要额外添加\0结束符，如果在前面使用过train会出现读取错误
//    memcpy(filename, train.data, train.length);
//    
//    int fd = open(filename, O_CREAT|O_RDWR, 0666);
//    while(1){    
//        recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){
//            break;
//        }
//        recv(sockfd, train.data, train.length, MSG_WAITALL);
//        write(fd, train.data, train.length);
//    }
//    close(fd);
//    return 0;
//}

//4.0 big file 加载进度
//int recvFile(int sockfd){
//    train_t train;
//    char filename[4096] = {0};
//    recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
//    recv(sockfd, train.data, train.length, MSG_WAITALL);
//    // 使用memcpy不需要额外添加\0结束符，如果在前面使用过train会出现读取错误
//    memcpy(filename, train.data, train.length);
//    
//    off_t filesize;
//    recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
//    recv(sockfd, train.data, train.length, MSG_WAITALL);
//    memcpy(&filesize, train.data, train.length);
//    printf("filesize = %ld\n", filesize);
//
//    off_t cursize = 0;
//    off_t lastsize = 0;
//    off_t slice = filesize/10000;
//    int fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
//    while(1){    
//        recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
//        if(train.length != 1000){
//            printf("train.length = %d\n", train.length);
//        }
//        if(train.length == 0){
//            break;
//        }
//        cursize += train.length;
//        recv(sockfd, train.data, train.length, MSG_WAITALL);
//        write(fd, train.data, train.length);
//        if(cursize - lastsize > slice){
//            printf("%5.2lf%%\r", cursize*100.0/filesize);
//            fflush(stdout);
//            lastsize = cursize;
//        }
//    }
//    printf("100.00%%\n");
//    close(fd);
//    return 0;
//}
//5.0 big file 使用mmap进行一次性接受大文件
int recvFile(int sockfd){
    train_t train;
    char filename[4096] = {0};
    recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
    recv(sockfd, train.data, train.length, MSG_WAITALL);
    // 使用memcpy不需要额外添加\0结束符，如果在前面使用过train会出现读取错误
    memcpy(filename, train.data, train.length);
    
    off_t filesize;
    recv(sockfd, &train.length, sizeof(train.length), MSG_WAITALL);
    recv(sockfd, train.data, train.length, MSG_WAITALL);
    memcpy(&filesize, train.data, train.length);
    printf("filesize = %ld\n", filesize);

    int fd = open(filename, O_CREAT|O_RDWR|O_TRUNC, 0666);
    ftruncate(fd, filesize);
    char* p = (char*)mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    recvn(sockfd, p, filesize);
    close(fd);
    return 0;
}
int main(int argc, char* argv[]){
    if (argc < 3){
        error(1, 1, "./client IP PORT");
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    int ret = connect(sockfd, (struct sockaddr*)&addr,sizeof(addr));
    if (ret == -1){
        error(1, errno, "connect error");
    }
    recvFile(sockfd);
    close(sockfd);
    return 0;
}
