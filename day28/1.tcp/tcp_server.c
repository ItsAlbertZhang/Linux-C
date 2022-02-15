#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    printf("请输入服务器 IP 地址与端口号, 以空格隔开.\n");
    char ipbuf[128] = {0};
    uint16_t hostport = 0;
    scanf("%s %hd", ipbuf, &hostport);
    // 1.socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.bind
    struct sockaddr_in server_addr;                                           //定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                 //清空结构体初始化
    server_addr.sin_family = AF_INET;                                         //使用 IPv4 通信协议
    server_addr.sin_port = htons(hostport);                                   //设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(ipbuf);                           //设置服务器 IP
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //绑定
    ERROR_CHECK(ret, -1, "bind");
    // 3.listen
    ret = listen(sockfd, 10); //开始监听
    ERROR_CHECK(ret, -1, "listen");
    // 4.accept
    struct sockaddr_in client_addr;           //定义 sockaddr_in 结构体
    bzero(&client_addr, sizeof(client_addr)); //清空结构体初始化
    socklen_t client_addrlen = sizeof(client_addr);
    printf("等待客户端连接...\n");
    int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen); //握手
    ERROR_CHECK(ret, -1, "accept");
    printf("连接成功, 客户端 IP 地址为 %s, 端口为 %hu.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    //收发数据
    //服务器接收数据
    char recvbuf[1024] = {0};
    ret = recv(new_fd, recvbuf, sizeof(recvbuf), 0);
    ERROR_CHECK(ret, -1, "recv");
    printf("接收到来自客户端的信息:\n%s\n", recvbuf);
    //服务器发送数据
    char sendbuf[1024] = "已收到你方发来数据.";
    ret = send(new_fd, sendbuf, strlen(sendbuf), 0);
    //关闭连接
    close(new_fd);
    close(sockfd);
    return 0;
}