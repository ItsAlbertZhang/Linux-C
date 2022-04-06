#ifndef __SEND_FILE_H__
#define __SEND_FILE_H__

#include "head.h"

// 服务器发送文件
int send_file(int connect_fd);
// 循环接收
size_t recv_n(int sockfd, void *buf, size_t len, int flags);

#endif /* __SEND_FILE_H__ */