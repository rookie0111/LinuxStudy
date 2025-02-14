#include"func.h"
int main(void){
   char cwd[20];
    // char* cwd;
    //first param is NULL, if size is  0, will calloc enough big menery,
    if (getcwd(NULL, 20) == NULL){
        // erreo
        //binfprintf(stderr, "getcwd erro");
        //perror("getcwd");
        //exit(1);
        //0 continue, not 0 exit
        error(1, errno, "getcwd");

    }

    // success
    puts(cwd);
    //free(cwd);
    return 0;
}
