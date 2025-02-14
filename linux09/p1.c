#include <func.h>
#define MAXLINE 256
int main(int argc, char* argv[]){

    int fd1 = open("pipe1", O_WRONLY);
    if (fd1 == -1)
        error(1, errno, "open pipe1");

    int fd2 = open("pipe2", O_RDONLY);
    if (fd2 == -1)
        error(1, errno, "open pipe2");

    printf("Established\n");

    // 定义接受数组
    char recvline[MAXLINE];
    char sendline[MAXLINE];

    // 设置感兴趣的fd位，以及最大fd, 感兴趣位置是会阻塞的文件描述符
    fd_set mainfds;
    FD_ZERO(&mainfds);
    FD_SET(STDIN_FILENO, &mainfds);
    int maxfds = STDIN_FILENO;

    FD_SET(fd2, &mainfds);
    if (fd2 > maxfds)
        maxfds = fd2;

    for(;;){
        fd_set readfds = mainfds; // copy struct

        // 设置select事件
        int events = select(maxfds + 1, &readfds, NULL, NULL, NULL);

        switch(events){
        case -1:
            error(1, errno, "select");
        case 0:
            printf("time out!\n");
            continue;
        default:
            if (FD_ISSET(STDIN_FILENO, &readfds)){
                fgets(sendline, MAXLINE, stdin);
                write(fd1, sendline, strlen(sendline)+1);
            }
            if(FD_ISSET(fd2, &readfds)){
                int nbytes =  read(fd2, recvline, MAXLINE);// return number of read bytes.
                if (nbytes == 0){
                    goto end; // 读取位0， 管道写端关闭，结束读取 
                }else if (nbytes == -1){
                    error(1, errno, "pipe2 error");
                }
                printf("from pipe2: %s", recvline);// enter 换行，不用单独加
            }
        }
    }
end:
    close(fd1);
    close(fd2);
    return 0;
}
