#include "process_parent.h"

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

int create_child_process(struct process_data_t *p_process_ctl, int process_num) {
    int ret = 0;

    bzero(p_process_ctl, sizeof(struct process_data_t) * process_num); // 初始化子进程控制信息块

    int pipe[2];   // 初始化一个数组, 用于创建全双工管道
    pid_t pid = 0; // 用于存放子进程 pid

    for (int i = 0; i < process_num; i++) {
        ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, pipe); // 创建全双工管道
        ERROR_CHECK(ret, -1, "socketpair");
        pid = fork(); // 创建子进程. 父进程返回子进程 pid, 子进程返回 0.

        //子进程
        if (0 == pid) {
            close(pipe[0]);    // 子进程使用 pipe[1] 与父进程通信
            child(pipe[1], i); // 子进程进入子进程处理函数, 并最终在其中执行 exit
        }

        close(pipe[1]); // 父进程使用 pipe[0] 与子进程通信
        p_process_ctl[i].pid = pid;
        p_process_ctl[i].pipefd = pipe[0];
        p_process_ctl[i].isbusy = 0;

        printf("child pid = %d, pipefd = %d.\n", pid, pipe[0]);
    }
}

int epoll_add(int epfd, int fd) {
    int ret = 0;

    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = fd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    return 0;
}

int new_connect(int sockfd, struct process_data_t *p_process_ctl) {
    int ret = 0;
    socklen_t addrlen = sizeof(p_process_ctl->addr);
    p_process_ctl->connect_fd = accept(sockfd, (struct sockaddr *)&p_process_ctl->addr, &addrlen); // 接受连接
    ERROR_CHECK(p_process_ctl->connect_fd, -1, "accept");
    printf("%s has connected.\n", inet_ntoa(p_process_ctl->addr.sin_addr));
    p_process_ctl->isbusy = 1;
    printf("process %d is busy.\n", p_process_ctl->pid);
    return 0;
}

int close_connect(struct process_data_t *p_process_ctl) {
    int ret = 0;
    bzero(&p_process_ctl->addr, sizeof(p_process_ctl->addr)); // 将子进程当前连接的用户信息清零
    p_process_ctl->connect_fd = -1;
    p_process_ctl->isbusy = 0;
    printf("process %d is free.\n", p_process_ctl->pid);
    return 0;
}

int sendfd(struct process_data_t *p_process_ctl) {
    int ret = 0;
    ret = write(p_process_ctl->pipefd, &p_process_ctl->connect_fd, sizeof(int));
    ERROR_CHECK(ret, -1, "write");
    close(p_process_ctl->connect_fd);
    return 0;
}
