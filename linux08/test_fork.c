#include "func.h"
int main(int argc, char* argv[])
{
    printf("Hello");
    // parent return  chirld pid, and chirld process return 0, error return -1 and errno.
    pid_t pid = fork();
    switch(pid){
    case -1:
        error(1, errno, "fork");
    case 0:
        printf("I am children process\n");
        printf("child_id = %d, parent_id = %d\n", getpid(), getppid());
        //exit(0);
        break;
    default:
        printf("I am father process\n");
        printf("child_id = %d, parent_id = %d\n", pid, getpid());
        sleep(1);
        break;
        //exit(0);
    }

    printf(" It is over!\n");
    return 0;
}
