#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        printf("./tcp_server ip port\n");
        return -1;
    }
    // 1.socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr;                                              //定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                    //清空结构体初始化
    server_addr.sin_family = AF_INET;                                            //使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(argv[2]));                                 //设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);                            //设置服务器 IP
    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //连接服务器
    ERROR_CHECK(ret, -1, "connect");
    //收发数据
    fd_set fd_set;  //定义一个 fd_set 集合
    char buf[1024]; //定义一个缓冲区
    while (1) {
        FD_ZERO(&fd_set); //重置 fd_set
        FD_SET(sockfd, &fd_set);
        FD_SET(STDIN_FILENO, &fd_set);
        select(sockfd + 1, &fd_set, NULL, NULL, NULL); //多路复用监听
        if (FD_ISSET(sockfd, &fd_set)) {
            //可以接收数据
            bzero(buf, sizeof(buf));
            ret = recv(sockfd, buf, sizeof(buf), 0); //从 sockfd 中接收数据
            if (!ret) {
                //返回异常
                printf("对方断开了连接.\n");
                break;
            } else {
                //返回正常
                printf("对方:%s\n", buf);
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
                ret = send(sockfd, buf, strlen(buf) - 1, 0);
            }
        }
    }
    //关闭连接
    close(sockfd);
    printf("已断开连接.\n");
    return 0;
}