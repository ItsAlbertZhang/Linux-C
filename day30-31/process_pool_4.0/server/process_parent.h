#ifndef process_parent_h
#define process_parent_h

#include <func.h>

struct process_data_t {
    pid_t pid;               // 子进程 pid
    int pipefd;              // 与子进程进行通信使用的管道文件描述符
    char isbusy;             // 子进程是否忙碌
    struct sockaddr_in addr; // 子进程当前连接用户的连接方式, IP 地址, 端口
    int connect_fd;          // 连接使用的 socket 描述符
};

// 初始化 tcp 环境, 执行 socket, bind, listen
int tcp_init(const char *server_ip, const char *port);
// 创建子进程
int create_child_process(struct process_data_t *p_process_ctl, int process_num);
// 添加 epoll 监听
int epoll_add(int epfd, int fd);
// 建立新连接
int new_connect(int sockfd, struct process_data_t *p_process_ctl);
// 关闭连接
int close_connect(struct process_data_t *p_process_ctl);
// 发送文件描述符(当前版本实际上只能发送值)
int sendfd(struct process_data_t *p_process_ctl);

#endif