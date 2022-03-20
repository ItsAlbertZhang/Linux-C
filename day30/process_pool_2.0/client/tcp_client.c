#include "func.h"

struct config_t {
    char ip[16];
    char port[8];
};

int get_a_new_line(FILE *f, char *s, int len) {
    char *ret = NULL;

    ret = fgets(s, len, f);
    s[strlen(s) - 1] = 0;
    if (ret == NULL) {
        printf("fgets failed.\n");
        return -1;
    }

    return 0;
}

int getconfig(FILE *configfile, struct config_t *p_conf) {
    int ret = 0;

    ret = get_a_new_line(configfile, p_conf->ip, sizeof(p_conf->ip));
    ERROR_CHECK(ret, -1, "get_a_new_line");
    ret = get_a_new_line(configfile, p_conf->port, sizeof(p_conf->port));
    ERROR_CHECK(ret, -1, "get_a_new_line");

    return 0;
}

int main(int argc, const char *argv[]) {
    int ret = 0;

    // 读取配置文件
    FILE *configfile = fopen("default.config", "rb");
    struct config_t conf;
    bzero(&conf, sizeof(conf));
    ret = getconfig(configfile, &conf);
    ERROR_CHECK(ret, -1, "getconfig");
    fclose(configfile);

    // 1.socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr;                                              // 定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                    // 清空结构体初始化
    server_addr.sin_family = AF_INET;                                            // 使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(conf.port));                               // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(conf.ip);                            // 设置服务器 IP
    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 连接服务器
    ERROR_CHECK(ret, -1, "connect");

    usleep(100);

    // 关闭连接
    close(sockfd);
    return 0;
}