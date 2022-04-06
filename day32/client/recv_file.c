/*
    协议:
    1. 客户端向服务器发送文件名长度, 服务器进行接收.
    2. 客户端向服务器发送文件名.
    3. 服务器向客户端发送文件大小, 客户端进行接收. 若文件不存在则文件大小为 0.
    4. 服务器向客户端发送文件内容.
*/
#include "recv_file.h"
#include "progress_bar.h"

// 总处理函数
int recv_file(int connect_fd, char *filename) {
    int ret = 0;

    // 第 1, 2 步
    ret = send_filename(connect_fd, filename);
    ERROR_CHECK(ret, -1, "recv_file");

    // 第 3, 4 步
    recv_file_content(connect_fd, filename);
}

// 第 1, 2 步
int send_filename(int connect_fd, char *filename) {
    int ret = 0;

    // 发送文件名长度
    size_t filename_len = strlen(filename);
    ret = send(connect_fd, &filename_len, sizeof(filename_len), 0);
    ERROR_CHECK(ret, -1, "send");

    // 发送文件名
    ret = send(connect_fd, filename, filename_len, 0);
    ERROR_CHECK(ret, -1, "send");

    return 0;
}

// 第 3, 4 步
int recv_file_content(int connect_fd, char *filename) {
    int ret = 0;

    // 接收文件大小
    size_t filesize = 0;
    ret = recv_n(connect_fd, &filesize, sizeof(filesize), 0);
    ERROR_CHECK(ret, -1, "recv_n");
    ERROR_CHECK(ret, 0, "recv_n"); // 返回值为 0 代表对端断开, 此时返回 -1 进而引发异常即可

    if (0 == filesize) {
        printf("服务器上没有该文件.\n");
    } else {
        int filefd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0666); // 打开文件
        // 初始化一条管道, 用于 splice 文件传输
        int pipefd[2] = {0};
        ret = pipe(pipefd);
        ERROR_CHECK(ret, -1, "pipe");
        ret = recv_splice_n(connect_fd, filefd, filesize, pipefd);
        ERROR_CHECK(ret, -1, "recv_splice_n");
    }

    return 0;
}

// splice 循环接收
size_t recv_splice_n(int connect_fd, int filefd, size_t filesize, int *pipefd) {
    size_t ret = 0;
    size_t recvsize = 0;
    struct progress_bar_t progress_bar;
    progress_bar_init(&progress_bar, filesize, 1);
    while (recvsize < filesize) {
        ret = splice(connect_fd, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        ret = splice(pipefd[0], NULL, filefd, NULL, ret, SPLICE_F_MORE | SPLICE_F_MOVE);
        recvsize += ret;
        progress_bar_handle(&progress_bar, recvsize);
    }
    progress_bar_destroy(&progress_bar);

    return recvsize;
}

// recv_n 循环接收
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