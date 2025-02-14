#include <func.h>
enum{
    FREE, 
    BUSSY
};

//define process info
typedef struct workerdata_s{
    pid_t pid;
    int status;
}workerdata_t;

int makeWorker(int workerNum, workerdata_t* workerArr);
