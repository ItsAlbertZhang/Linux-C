#include "process_child.h"
#include "process_parent.h"

// 子进程句柄函数
int child(const int pipe, const int id) {
    int ret = 0;
    int epfd = epoll_create(1);  // 创建 epoll 句柄
    ret = epoll_add(epfd, pipe); // 将与父进程通信的管道添加至 epoll 监听
    ERROR_CHECK(ret, -1, "epoll_add");
    struct epoll_event events;
    bzero(&events, sizeof(events));

    int connect_fd = 0;
    int PROGRAM_RUNNING = 1;
    while (PROGRAM_RUNNING) {
        ret = epoll_wait(epfd, &events, 1, -1);
        if (ret > 0) {
            ret = recvfd(pipe, &connect_fd);
            ERROR_CHECK(ret, -1, "recvfd");

            char filename[128] = {0};                              // 客户端请求文件名
            ret = recv(connect_fd, filename, sizeof(filename), 0); // 从客户端接收请求文件名
            ERROR_CHECK(ret, -1, "recv");
            printf("filename = %s\n", filename);

            ret = transfile(connect_fd, filename);
            // 特殊处理机制, 当对端断开时不崩溃, 并重新开始任务
            if (-1 == ret) {
                printf("child %d: Connection lost.\n", getpid());
            }

            close(connect_fd);
            write(pipe, &id, sizeof(id)); // 告知父进程当前任务已完成
        }
    }
    return 0;
}

// 从父进程接取连接文件描述符的内核控制信息, 与客户端建立连接
int recvfd(int pipefd, int *connect_fd) {
    int ret = 0;

    struct msghdr msg; // 定义用于 sendmsg 的结构体
    bzero(&msg, sizeof(msg));
    // 操作 msg 结构体中的 iovec 结构体成员
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    struct iovec iov;
    iov.iov_base = &addr;
    iov.iov_len = sizeof(addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    // 操作 msg 结构体中的 void *msg_control 变长结构体成员, 该成员往往定义为 struct cmsghdr * 类型
    int cmsg_length = CMSG_LEN((sizeof(int)));
    struct cmsghdr *cmsg = (struct cmsghdr *)malloc(cmsg_length);
    cmsg->cmsg_len = cmsg_length;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    // 将 msg_control 变长结构体成员填入 struct msghdr msg 结构体
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg_length;

    ret = recvmsg(pipefd, &msg, 0);
    ERROR_CHECK(ret, -1, "recvmsg");
    *connect_fd = *(int *)CMSG_DATA(cmsg); // 获取要传递的文件描述符

    printf("child %d: %s has connected, start working...\n", getpid(), inet_ntoa(addr.sin_addr));
    free(cmsg);

    return 0;
}

// 向客户端发送文件
int transfile(int connect_fd, const char *filename) {
    int ret = 0;

    struct trans_t tr;
    bzero(&tr, sizeof(tr));
    size_t filesize = 0;
    int filefd = open(filename, O_RDONLY);
    // 发送文件大小 (为 0 表示不存在该文件)
    if (-1 == filefd) { // 服务器不存在该文件
        filesize = 0;
    } else { // 服务器存在该文件
        // 获取文件状态
        struct stat filestat;
        bzero(&filestat, sizeof(filestat));
        ret = stat(filename, &filestat);
        ERROR_CHECK(ret, -1, "stat");
        filesize = filestat.st_size;
    }
    ret = send(connect_fd, &filesize, sizeof(filesize), 0);
    ERROR_CHECK(ret, -1, "send");
    // 发送文件
    if (filesize) {
        // char *filebuf = (char *)mmap(NULL, filesize, PROT_READ, MAP_SHARED, filefd, 0);
        // ret = send_n(connect_fd, filebuf, filesize, 0);
        // ERROR_CHECK(ret, -1, "send_n");
        // munmap(filebuf, filesize);
        ret = sendfile(connect_fd, filefd, 0, filesize);
        ERROR_CHECK(ret, -1, "sendfile");
        close(filefd);
    }
    return 0;
}

// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;

    char *p = (char *)buf;
    while (len > 0) {
        ret = recv(sockfd, p, len, flags);
        ERROR_CHECK(ret, -1, "recv");
        p += ret;
        len -= ret;
    }

    return 0;
}

// 循环发送
int send_n(int sockfd, const void *buf, size_t len, int flags) {
    int ret = 0;

    const char *p = (char *)buf;
    while (len > 0) {
        ret = send(sockfd, p, len, flags);
        ERROR_CHECK(ret, -1, "send");
        p += ret;
        len -= ret;
    }

    return 0;
}