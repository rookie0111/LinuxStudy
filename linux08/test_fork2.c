#include "func.h"
int main(int argc, char* argv[])
{

    int fd = open("./text.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
    // printf("Hello");
    // parent return  chirld pid, and chirld process return 0, error return -1 and errno.
    pid_t pid = fork();
    int newfd;
    printf("pos1 : %ld\n", lseek(fd, 0, SEEK_CUR));
    switch(pid){
    case -1:
        error(1, errno, "fork");
    case 0:
        printf("I am children process\n");
        printf("child_id = %d, parent_id = %d\n", getpid(), getppid());
        //exit(0);
        write(fd, "Hello Wold\n", 11);
        printf("pos2 : %ld\n", lseek(fd, 0, SEEK_CUR));
        close(STDIN_FILENO);
        close(STDERR_FILENO);
        newfd = dup(fd);
        printf("clild_new_fd = %d\n", newfd); // newid == 1
        exit(0);
    default:
        sleep(3);
        printf("I am father process\n");
        printf("child_id = %d, parent_id = %d\n", pid, getpid());
        newfd = dup(fd);
        printf("pos3 : %ld\n", lseek(fd, 0, SEEK_CUR));
        printf("parent_new_fd = %d\n", newfd);// newid == 4
        exit(0);
        //exit(0);
    }

    printf(" It is over!\n");
    return 0;
}
