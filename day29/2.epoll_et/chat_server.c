#include <func.h>
//使用 epoll 时采用边缘触发与非阻塞模型

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
    // 2.bind
    struct sockaddr_in server_addr; //定义一个用于 bind 的结构体
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    int reuseaddr = 1; //设置端口重用
    ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    ERROR_CHECK(ret, -1, "setsockopt");
    ret = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //执行 bind
    ERROR_CHECK(ret, -1, "bind");
    // 3.listen
    ret = listen(sfd, 10);

    // 4.accept
    struct sockaddr_in client_addr; //定义一个用于接收储存客户端信息的结构体
    bzero(&client_addr, sizeof(client_addr));
    socklen_t client_addrlen = sizeof(client_addr);
    int new_fd = accept(sfd, (struct sockaddr *)&client_addr, &client_addrlen);
    ERROR_CHECK(new_fd, -1, "accept");

    // 5.收发数据

    // epoll 句柄的创建
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    // epoll 句柄的注册
    struct epoll_event epevt;
    bzero(&epevt, sizeof(epevt));
    // 注册标准输入
    epevt.events = EPOLLIN | EPOLLET; //边缘触发
    epevt.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &epevt);
    ERROR_CHECK(ret, -1, "ret");
    // 注册通信文件描述符
    epevt.events = EPOLLIN | EPOLLET; //边缘触发
    epevt.data.fd = new_fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &epevt);
    ERROR_CHECK(ret, -1, "ret");
    // 进行监听前准备
    char buf[5] = {0};
    int epoll_ready_num = 0;
    struct epoll_event epevts[2];
    bzero(epevts, sizeof(epevts));
    //修改 STDIN_FILENO 和 new_fd 为非阻塞模型
    ret = fcntl(STDIN_FILENO, F_GETFL);
    ret = fcntl(STDIN_FILENO, F_SETFL, ret | O_NONBLOCK);
    ret = fcntl(new_fd, F_GETFL);
    ret = fcntl(new_fd, F_SETFL, ret | O_NONBLOCK);
    //监听
    while (1) {
        epoll_ready_num = epoll_wait(epfd, epevts, 2, -1);
        ERROR_CHECK(-1, epoll_ready_num, "epoll_wait");
        if (0 == epoll_ready_num) {
            printf("Timeout\n");
        }
        for (int i = 0; i < epoll_ready_num; i++) {
            if (epevts[i].data.fd == new_fd) {
                // bzero(buf, sizeof(buf));
                // ret = recv(new_fd, buf, sizeof(buf), 0);
                // if (0 == ret) {
                //     printf("对方断开了连接.\n");
                //     close(new_fd);
                //     close(sfd);
                //     return 0;
                // } else {
                //     printf("recv = %s\n", buf);
                // }
                while (1) {
                    bzero(buf, sizeof(buf));
                    ret = recv(new_fd, buf, sizeof(buf) - 1, 0);
                    if (-1 == ret) { //读空
                        printf("\n");
                        break;
                    } else if (0 == ret) {
                        printf("对方断开了连接.\n");
                        close(new_fd);
                        close(sfd);
                        return 0;
                    } else { //循环读取
                        printf("%s", buf);
                    }
                }
            }
            if (epevts[i].data.fd == STDIN_FILENO) {
                // bzero(buf, sizeof(buf));
                // ret = read(STDIN_FILENO, buf, sizeof(buf));
                // if (0 == ret) {
                //     printf("正在断开连接...\n");
                //     close(new_fd);
                //     close(sfd);
                //     printf("已断开连接.\n");
                //     return 0;
                // } else {
                //     ret = send(new_fd, buf, strlen(buf) - 1, 0);
                //     ERROR_CHECK(ret, -1, "send");
                // }
                while (1) {
                    bzero(buf, sizeof(buf));
                    ret = read(STDIN_FILENO, buf, sizeof(buf));
                    if (-1 == ret) { //读空
                        break;
                    } else if (0 == ret) {
                        printf("正在断开连接...\n");
                        close(new_fd);
                        close(sfd);
                        printf("已断开连接.\n");
                        return 0;
                    } else { //循环读取
                        ret = send(new_fd, buf, sizeof(buf), 0);
                        ERROR_CHECK(-1, ret, "send");
                    }
                }
            }
        }
    }
    // 6.断开连接
    close(new_fd);
    close(sfd);

    return 0;
}