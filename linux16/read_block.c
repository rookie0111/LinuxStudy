#include <func.h>
int SetNonblock(int fd){
    int flag = fcntl(fd, F_GETFL);
    printf("before flag = %d\n",flag); 
    flag = flag| O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flag);
    printf("after flag = %d\n",flag); 
    if (ret == -1){
        error(1, errno, "fcntl error");
    }
    return 0;
}

int main(int argc, char* argv[]){
    int fd = open("1.pipe", O_RDONLY);
    SetNonblock(fd);
    char buff[3] = {0};
    while(1){
        bzero(buff, 0);
        ssize_t ret = read(fd, buff, 2);
        if (ret == 0){
            break;
        }
        printf("ret = %ld, buff = %s \n", ret, buff);
        sleep(1);
    }
    close(fd);
    return 0;
}
