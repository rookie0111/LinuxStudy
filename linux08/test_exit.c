#include "func.h"

void func(void){
    printf("func will be died...\n");
}

int main(int argc, char* argv[]){
    int err = atexit(func);
    if(err){
        error(1, 0, "atexit");
    }
    printf("I am main func\n");
    exit(100);
    return 0;
}
