#include"func.h"
int main(int argc, char* argv[]){
    // ./after change path
    if (argc != 2){
        // 没有输入路径，是输入错误，不需要设置errno
        error(1, 0, "usage: %s path", argv[0]);
    }

    char cwd[128];
    getcwd(cwd, 128);
    puts(cwd);

    // Idiom :惯用法
    if (chdir(argv[1]) == -1){
        error(1, errno, "chdir(%s)", argv[1]);
    }
  
    getcwd(cwd, 128);
    puts(cwd);
    return 0;
}
