/*
    协议:
    1. 客户端向服务器发送文件名长度, 服务器进行接收.
    2. 客户端向服务器发送文件名.
    3. 服务器向客户端发送文件大小, 客户端进行接收. 若文件不存在则文件大小为 0.
    4. 服务器向客户端发送文件内容.
*/
#include "send_file.h"

int send_file(int connect_fd) {
    int ret = 0;

    // 接收来自客户端的文件名长度
    size_t filename_len = 0;
    ret = recv_n(connect_fd, &filename_len, sizeof(filename_len), 0);
    RETURN_ZERO_CHECK(ret, -1); // 如果对端断开, 则直接返回 0
    // 接收来自客户端的文件名
    char filename[64] = {0};
    ret = recv_n(connect_fd, filename, filename_len, 0);
    ERROR_CHECK(ret, -1, "recv_n");
    RETURN_ZERO_CHECK(ret, -1); // 如果对端断开, 则直接返回 0

    // 向客户端发送文件大小
    size_t filesize = 0;
    int filefd = open(filename, O_RDONLY); // 尝试打开文件
    if (-1 == filefd) {
        // 文件不存在
        filesize = 0;
    } else {
        // 文件存在, 获取其属性
        struct stat filestat;
        bzero(&filestat, sizeof(filestat));
        ret = stat(filename, &filestat);
        ERROR_CHECK(ret, -1, "stat");
        filesize = filestat.st_size;
    }
    ret = send(connect_fd, &filesize, sizeof(filesize), 0);

    // 向客户端发送文件
    if (-1 != ret) {
        sendfile(connect_fd, filefd, NULL, filesize);
        // 无需检查返回值, 当连接断开时可直接结束
    }
    close(filefd);

    return 0;
}

size_t recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;

    char *p = (char *)buf;
    size_t recved_len = 0;
    while (recved_len < len) {
        ret = recv(sockfd, p + recved_len, len - recved_len, flags);
        ERROR_CHECK(ret, -1, "recv");
        RETURN_ZERO_CHECK(ret, 0); // 当对端断开时, 返回 0
        recved_len += ret;
    }

    return recved_len;
}