#include "func.h"
#include <mysql/mysql.h>
int main(int argc, char* argv[]){
    MYSQL* mysql = mysql_init(NULL);
    MYSQL* cret = mysql_real_connect(mysql, "192.168.101.127", "root", "111", "example", 0, NULL, 0);
    if (cret == NULL){
        fprintf(stderr, "mysql_real_connect:%s\n", mysql_error(mysql));
        return -1;
    }
    printf("start query\n");
    char sql[4096] = "select * from dept;";
    int qret = mysql_query(mysql, sql);
    printf("query finished!\n");
    if (qret != 0){
        fprintf(stderr, "mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    MYSQL_RES *res = mysql_store_result(mysql);// 存下多行结果
    printf("total row:%lu, total col:%d\n", mysql_num_rows(res), mysql_num_fields(res));
    MYSQL_ROW row;// one line string result.
    while((row = mysql_fetch_row(res))!=NULL){
        for(unsigned int i=0; i<mysql_num_fields(res); ++i){
            printf("%s\t", row[i]);
        }
        printf("\n");
    }
    mysql_close(mysql);
    return 0;
}
