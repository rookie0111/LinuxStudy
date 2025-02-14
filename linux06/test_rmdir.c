#include"func.h"
int main(int argc, char* argv[]){
    // ./test_rmdir dir
    if (argc != 2){
        error(1, 0, "usage: %s dir", argv[0]);
    }
    if (rmdir(argv[1]) == -1){
        error(1, errno, "rmdir %s", argv[1]);
    }

    return 0;
}
