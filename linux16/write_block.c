#include <func.h>
int main(int argc, char* argv[]){
    int fd = open("1.pipe", O_WRONLY);
    printf("fd = %d\n", fd);
    write(fd, "hello world", 11);
    sleep(15);
    printf("sleep over!\n");
    close(fd);
    return 0;
}

