#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        printf("./tcp_client ip port\n");
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
    //客户端发送数据
    char sendbuf[1024] = {0};
    printf("请输入要发送的文本:\n");
    fflush(stdin);
    read(STDIN_FILENO, sendbuf, sizeof(sendbuf));
    ret = send(sockfd, sendbuf, strlen(sendbuf) - 1, 0);
    //客户端接收数据
    char recvbuf[1024] = {0};
    ret = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
    ERROR_CHECK(ret, -1, "recv");
    printf("接收到来自服务器的信息:\n%s\n", recvbuf);
    //关闭连接
    close(sockfd);
    return 0;
}