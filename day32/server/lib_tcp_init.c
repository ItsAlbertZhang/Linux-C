#include "lib_tcp_init.h"

int tcp_init(const char *server_ip, const char *server_port) {
    int ret = 0;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // socket
    ERROR_CHECK(sockfd, -1, "socket");

    // bind 前准备
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(atoi(server_port));
    // 设置端口重用
    int reuseaddr = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    ERROR_CHECK(ret, -1, "setsockopt");
    // 执行 bind
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, -1, "bind");

    ret = listen(sockfd, 10); // 执行 listen
    ERROR_CHECK(ret, -1, "listen");

    return sockfd;
}