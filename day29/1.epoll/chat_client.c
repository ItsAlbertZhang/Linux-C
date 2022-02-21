#include <func.h>
//使用 epoll 代替 select 作为多路复用机制.

int main(int argc, const char *argv[]) {
    //程序自检
    int ret = 0;
    if (argc != 3) {
        printf("./tcp_server ip port\n");
        return -1;
    }

    // 1.socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr; //定义一个用于 connect 的结构体
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    ret = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //执行 connect
    ERROR_CHECK(ret, -1, "connect");

    // 3.收发数据

    // epoll 句柄的创建
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    // epoll 句柄的注册
    struct epoll_event epevt;
    bzero(&epevt, sizeof(epevt));
    // 注册标准输入
    epevt.events = EPOLLIN;
    epevt.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &epevt);
    ERROR_CHECK(ret, -1, "ret");
    // 注册通信文件描述符
    epevt.events = EPOLLIN;
    epevt.data.fd = sfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &epevt);
    ERROR_CHECK(ret, -1, "ret");
    // 进行监听前准备
    char buf[1024] = {0};
    int epoll_ready_num = 0;
    struct epoll_event epevts[2];
    bzero(epevts, sizeof(epevts));
    //监听
    while (1) {
        epoll_ready_num = epoll_wait(epfd, epevts, 2, -1);
        ERROR_CHECK(-1, epoll_ready_num, "epoll_wait");
        if (0 == epoll_ready_num) {
            printf("Timeout\n");
        }
        for (int i = 0; i < epoll_ready_num; i++) {
            if (epevts[i].data.fd == sfd) {
                bzero(buf, sizeof(buf));
                ret = recv(sfd, buf, sizeof(buf), 0);
                if (0 == ret) {
                    printf("对方断开了连接.\n");
                    close(sfd);
                    return 0;
                } else {
                    printf("recv = %s\n", buf);
                }
            }
            if (epevts[i].data.fd == STDIN_FILENO) {
                bzero(buf, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf));
                if (0 == ret) {
                    printf("正在断开连接...\n");
                    close(sfd);
                    printf("已断开连接.\n");
                    return 0;
                } else {
                    ret = send(sfd, buf, strlen(buf) - 1, 0);
                    ERROR_CHECK(ret, -1, "send");
                }
            }
        }
    }
    // 6.断开连接
    close(sfd);

    return 0;
}