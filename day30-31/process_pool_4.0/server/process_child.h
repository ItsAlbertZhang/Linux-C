#ifndef process_child_h
#define process_child_h

#include <func.h>

struct trans_t {
    off_t buflen;
    char buf[1024];
};

// 从父进程接取连接文件描述符的内核控制信息
int recvfd(int pipefd, int *connect_fd);

// 向客户端发送文件
int transfile(int connect_fd, const char *filename);

#endif