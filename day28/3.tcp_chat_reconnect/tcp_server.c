#include <func.h>
//通过在 tcp 通信中加入 select 多路复用机制来实现即时聊天
//用 select 监听 sockfd, 并将 accept 放入循环内以实现服务器端不间断运行, 客户端可以重连的效果

//已知问题: 对于客户端连接未做链表处理, 因此在客户端断开时只能更改其编号. 使用链表或队列并更改辨别设计可以解决此问题.
//将在使用epoll多路复用机制实现多人即时在线聊天工具中解决这一问题.

#define MAX_CONNECT 10

int main(int argc, const char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        printf("./tcp_server ip port\n");
        return -1;
    }
    // 1.socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.bind
    int reuseaddr = 1;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)); //设置地址和端口重用
    ERROR_CHECK(ret, -1, "setsockopt");
    struct sockaddr_in server_addr;                                           //定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                 //清空结构体初始化
    server_addr.sin_family = AF_INET;                                         //使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(argv[2]));                              //设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);                         //设置服务器 IP
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //绑定
    ERROR_CHECK(ret, -1, "bind");
    // 3.listen
    ret = listen(sockfd, 10); //开始监听
    ERROR_CHECK(ret, -1, "listen");
    // 4.accept 的初始化部分 (定义用于 accept 的变量)
    struct sockaddr_in client_addr; //定义 sockaddr_in 结构体
    socklen_t client_addrlen = sizeof(client_addr);
    int new_fd[MAX_CONNECT] = {0};
    int connect_fd = 0;
    printf("等待客户端连接...\n");
    //收发数据
    fd_set fd_set;  //定义一个 fd_set 集合
    char buf[1024]; //定义一个缓冲区
    while (1) {
        FD_ZERO(&fd_set); //重置 fd_set
        for(int i = 0; i < connect_fd; i++) {
            FD_SET(new_fd[i], &fd_set);
        }
        FD_SET(STDIN_FILENO, &fd_set);
        FD_SET(sockfd, &fd_set);
        select(MAX_CONNECT + 4, &fd_set, NULL, NULL, NULL); //多路复用监听
        if(FD_ISSET(sockfd, &fd_set)) {
            //有新连接
            bzero(&client_addr, sizeof(client_addr));
            new_fd[connect_fd] = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen); //握手
            ERROR_CHECK(ret, -1, "accept");
            connect_fd++;
            printf("连接成功, 客户端 IP 地址为 %s, 端口为 %hu.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        }
        for(int i = 0; i < connect_fd; i++) {
            if (FD_ISSET(new_fd[i], &fd_set)) {
                //可以接收数据
                bzero(buf, sizeof(buf));
                ret = recv(new_fd[i], buf, sizeof(buf), 0); //从 new_fd 中接收数据
                if (!ret) {
                    //返回异常
                    printf("%d号客户端断开了连接. 所有编号大于 %d 的客户端编号减 1.\n", i, i);
                    close(new_fd[i]);
                    for (int j = i; j < connect_fd - 1; j++) {
                        new_fd[j] = new_fd[j + 1];
                    }
                    connect_fd--;
                    break;
                } else {
                    //返回正常
                    printf("%d号客户端:%s\n", i, buf);
                }
            }
        }
        if (FD_ISSET(STDIN_FILENO, &fd_set)) {
            //可以发送数据
            bzero(buf, sizeof(buf));
            ret = read(STDIN_FILENO, buf, sizeof(buf)); //从标准输入缓冲区中读数据
            if (!ret) {
                //返回异常
                printf("正在断开连接...\n");
                break;
            } else {
                //返回正常
                for(int i = 0; i < connect_fd; i++) {
                    ret = send(new_fd[i], buf, strlen(buf) - 1, 0);
                }
            }
        }
    }
    //关闭连接
    for(int i = 0; i < connect_fd; i++) {
        close(new_fd[i]);
    }
    close(sockfd);
    printf("已断开连接.\n");
    return 0;
}