#include <func.h>

int main(int argc, char* argv[]){
    uid_t uid;// 真实用户ID unsigned int type
    gid_t gid;// 真实组ID
    uid_t euid;// 有效用户ID
    gid_t egid;// 有效组ID
    uid = getuid();
    gid = getgid();
    euid = geteuid();
    egid = getegid();
    printf("getpid = %d, getppid = %d, uid = %d, gid = %d, euid = %d, egid = %d\n",
           getpid(), getppid(), uid, gid, euid, egid);
    return 0;
}
