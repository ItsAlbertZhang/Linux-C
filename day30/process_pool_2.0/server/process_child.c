#include "process_child.h"

// 从父进程接取连接文件描述符的内核控制信息
int recvfd(int pipefd, int *connect_fd) {
    int ret = 0;

    struct msghdr msg; // 定义用于 recvmsg 的结构体
    bzero(&msg, sizeof(msg));
    // 操作 msg 结构体中的 void *msg_control 变长结构体成员, 该成员往往定义为 struct cmsghdr * 类型
    int cmsg_length = CMSG_LENGTH((sizeof(int)));
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

    // 在本实例中, 没有使用 iovec 结构体数组指针传递数据

    free(cmsg);

    return 0;
}