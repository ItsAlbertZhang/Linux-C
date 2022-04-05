#include "lib_tcp_connect.h"

int tcp_connect(const char *server_ip, const char *server_port) {
    int ret = 0;

    // 1.socket
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");

    // 2.connect
    struct sockaddr_in server_addr;                                                  // 定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                        // 清空结构体初始化
    server_addr.sin_family = AF_INET;                                                // 使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(server_port));                                 // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(server_ip);                              // 设置服务器 IP
    ret = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 连接服务器
    ERROR_CHECK(ret, -1, "connect");

    return connect_fd;
}