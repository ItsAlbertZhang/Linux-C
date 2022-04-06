#ifndef __RECV_FILE_H__
#define __RECV_FILE_H__

#include "head.h"

// 总处理函数
int recv_file(int connect_fd, char *filename);

// 第 1, 2 步
int send_filename(int connect_fd, char *filename);

// 第 3, 4 步
int recv_file_content(int connect_fd, char *filename);

// splice 循环接收
size_t recv_splice_n(int connect_fd, int filefd, size_t filesize, int *pipefd);

// recv_n 循环接收
size_t recv_n(int sockfd, void *buf, size_t len, int flags);

#endif /* __RECV_FILE_H__ */