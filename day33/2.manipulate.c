#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct config_t {
    char server[50];
    char user[50];
    char password[50];
    char database[50];
};

int get_a_new_line(FILE *f, char *s) {
    char *ret = NULL;
    ret = fgets(s, 50, f);
    if (ret == NULL) {
        return -1;
    } else {
        s[strlen(s) - 1] = 0; // 替换行尾的 \n 为 \0
    }
    return 0;
}

void getconfig(char *config_filename, struct config_t *conf) {
    int ret = 0;
    FILE *config_file = fopen(config_filename, "rb"); // 打开配置文件
    if (NULL == config_file) {
        printf("[ERROR] %s:%d 配置文件不存在.\n", __FILE__, __LINE__);
        abort();
    } else {
        printf("[INFO] 正在读取配置文件...\n");
        for (int i = 0; i < 4; i++) {
            ret = get_a_new_line(config_file, (char *)conf + 50 * i);
            if (-1 == ret) {
                printf("[ERROR] %s:%d 配置文件错误.\n", __FILE__, __LINE__);
                abort();
            }
        }
        printf("[INFO] 成功读取配置文件.\n");
        fclose(config_file);
    }
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    struct config_t conf;
    getconfig("default.config", &conf);
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, conf.server, conf.user, conf.password, conf.database, 0, NULL, 0)) {
        printf("[ERROR] %s:%d 连接 MySQL 数据库时发生错误: %s\n", __FILE__, __LINE__, mysql_error(conn));
        return -1;
    } else {
        printf("[INFO] 成功连接到 MySQL 数据库.\n");
        char query[300];
        ssize_t querylen = 0;
        unsigned int query_ret;
        char nodata = 1;
        while (1) {
            querylen = read(STDIN_FILENO, query, sizeof(query));
            query[querylen - 1] = 0;
            if (!strcmp(query, "exit")) {
                break;
            } else {
                printf("%s\n", query);
                query_ret = mysql_query(conn, query);
                if (query_ret) {
                    printf("[ERROR] %s:%d MySQL 语法错误: %s\n", __FILE__, __LINE__, mysql_error(conn));
                } else {
                    // // DQL 数据查询语句, 显示查询结果.
                    // res = mysql_use_result(conn);
                    // if (res) {
                    //     while ((row = mysql_fetch_row(res)) != NULL) {
                    //         nodata = 0;
                    //         for (int i = 0; i < mysql_num_fields(res); i++) {
                    //             printf("%10s ", row[i]);
                    //         }
                    //         printf("\n");
                    //     }
                    //     if (nodata) {
                    //         printf("[INFO] 没有找到数据.\n");
                    //     } else {
                    //         nodata = 1;
                    //     }
                    // }
                    // mysql_free_result(res);
                    // DML 数据操作语句, 显示影响行数.
                    printf("affected rows: %ld.\n", (long)mysql_affected_rows(conn));
                }
            }
        }
        mysql_close(conn);
    }
    return 0;
}