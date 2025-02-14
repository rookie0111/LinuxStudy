#include "func.h"
int main(int argc, char* argv[]){
    if (argc != 3){
        error(1, 0, "Usage: %s mode file", argv[0]);
    }
    mode_t mode;
    // format input octal number.
    sscanf(argv[1], "%o", &mode);
    if (chmod(argv[2], mode)!=0){
        error(1, errno, "chmod failed");
    }

    return 0;
}
