#include"func.h"
int main(int argc, char* argv[]){
    // usage: ./test_mkdir dir mode
    // 参数校验
    if (argc != 3){
        error(1, 0, "usage: %s dir mode", argv[0]);
    }
    // 参数类型转换
    mode_t mode;
    // format input
    sscanf(argv[2], "%o", &mode);
    int err = mkdir(argv[1], mode);
    // not zero is error
    if (err){
        error(1, errno, "mkdir %s", argv[1]);
    }
    return 0;
}
