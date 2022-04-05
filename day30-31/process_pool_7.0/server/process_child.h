#ifndef process_child_h
#define process_child_h

#include <func.h>

struct trans_t {
    size_t buflen;
    char buf[1024];
};

// 子进程句柄函数
int child(const int pipe, const int id);
// 从父进程接取连接文件描述符的内核控制信息
int recvfd(int pipefd, int *connect_fd);
// 向客户端发送文件
int transfile(int connect_fd, const char *filename);
// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags);
// 循环发送
int send_n(int sockfd, const void *buf, size_t len, int flags);


#endif