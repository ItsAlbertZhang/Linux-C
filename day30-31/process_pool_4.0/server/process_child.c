#include "process_child.h"

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
    int filefd = open(filename, O_RDONLY);
    if (-1 == filefd) { // 服务器不存在该文件
        tr.buflen = -1; // 向客户端发送的第一个数据包中, 数据包的大小设置为 -1.
        ret = send(connect_fd, &tr, sizeof(tr.buflen), 0);
        ERROR_CHECK(ret, -1, "send");
    } else { // 服务器存在该文件
        // 获取文件状态
        struct stat filestat;
        bzero(&filestat, sizeof(filestat));
        ret = stat(filename, &filestat);
        // 发送文件
        off_t restsize = filestat.st_size; // 剩余未发送大小
        while (restsize > 0) {
            restsize -= tr.buflen;
            tr.buflen = read(filefd, tr.buf, sizeof(tr.buf));
            ERROR_CHECK(tr.buflen, -1, "read");
            ret = send_n(connect_fd, &tr, sizeof(tr.buflen) + tr.buflen, 0);
            ERROR_CHECK(ret, -1, "send");
        }
    }
    return 0;
}

// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;

    char *p = (char *)buf;
    while(len > 0) {
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
    while(len > 0) {
        ret = send(sockfd, p, len, flags);
        ERROR_CHECK(ret, -1, "send");
        p += ret;
        len -= ret;
    }
    
    return 0;
}