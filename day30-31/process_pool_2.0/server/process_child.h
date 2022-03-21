#ifndef process_child_h
#define process_child_h

#include <func.h>

// 从父进程接取连接文件描述符的内核控制信息
int recvfd(int pipefd, int *connect_fd);

#endif