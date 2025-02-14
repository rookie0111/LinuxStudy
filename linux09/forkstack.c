#include <func.h>
int main(){
    pid_t pid =fork();
    char* p = (char*)malloc(6);
    strcpy(p,"hello");
    switch(pid){
        case -1: 
            error(1, errno, "fork");
        case 0:
            puts("child");
            printf("child p = %s, p = %p", p, p);
            strcpy(p, "world");
            printf("child p = %s, p = %p", p, p);
            break;
        default:
            puts("parent");
            printf("prent p = %s, p = %p", p, p);
            sleep(1);
            printf("prent p = %s, p = %p", p, p);
            break;
    }
    return 0;
}
