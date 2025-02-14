#include<func.h>
int main(int argc, char* argv[]){
    int fds[2];
    //pipe(fds);
    socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
    if (fork()){// father process
        close(fds[0]);
        int fdfile = open("file1", O_RDWR);
        printf("parent fdfile = %d\n", fdfile);
        //write(fdfile, "hello", 5);
        send(fdfile, "hello", 5, 0);
        //write(fds[1], &fdfile, sizeof(int));
        send(fds[1], &fdfile, sizeof(int), 0);
        wait(NULL);
    }
    else{
        close(fds[1]);
        int fdfile;
        //read(fds[0], &fdfile, sizeof(int));
        recv(fds[0], &fdfile, sizeof(int), 0);
        printf("child fdfile = %d\n", fdfile);
        ssize_t sret = write(fdfile, "world", 5);
        printf("sret = %ld\n", sret);
    }
    return 0;
}
