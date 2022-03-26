#include "func.h"

struct config_t {
    char ip[16];
    char port[8];
    char filename[128];
};

struct trans_t {
    off_t buflen;
    char buf[1024];
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
    ret = get_a_new_line(configfile, p_conf->filename, sizeof(p_conf->filename));
    ERROR_CHECK(ret, -1, "get_a_new_line");

    return 0;
}

// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;

    char *p = (char *)buf;
    while(len > 0) {
        ret = recv(sockfd, p, len, flags);
        ERROR_CHECK(ret, -1, "recv");
        p += ret;
        len -= ret;
    }
    
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
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr;                                                  // 定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                        // 清空结构体初始化
    server_addr.sin_family = AF_INET;                                                // 使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(conf.port));                                   // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(conf.ip);                                // 设置服务器 IP
    ret = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 连接服务器
    ERROR_CHECK(ret, -1, "connect");

    struct trans_t tr;
    printf("conf.filename = %s\n", conf.filename);
    int filefd = open(conf.filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
    ERROR_CHECK(filefd, -1, "open");
    printf("filefd = %d\n", filefd);
    ret = send(connect_fd, conf.filename, strlen(conf.filename), 0);
    ERROR_CHECK(ret, -1, "send");
    while (1) {
        ret = recv(connect_fd, &tr.buflen, sizeof(tr.buflen), 0); // 先接收结构体的头部
        ERROR_CHECK(ret, -1, "recv");
        if (-1 == tr.buflen) { // 服务器上没有该文件, 接收的结构体头部为 -1
            close(filefd);
            ret = unlink(conf.filename);
            ERROR_CHECK(ret, -1, "unlink");
            printf("No such file exists on the server.\n");
            break;
        } else if (0 == tr.buflen) { // 服务器发送完毕, 接收的结构体头部为 0
            close(filefd);
            printf("Received successfully!\n");
            break;
        } else { // 正常接收文件, 接收的结构体头部为接收内容大小
            ret = recv_n(connect_fd, &tr.buf, tr.buflen, 0); // 接收结构体的主体部分
            ERROR_CHECK(ret, -1, "recv");
            ret = write(filefd, tr.buf, tr.buflen); // 写入文件
            ERROR_CHECK(ret, -1, "write");
            tr.buflen = 0;
        }
    }

    // 关闭连接
    close(connect_fd);
    return 0;
}