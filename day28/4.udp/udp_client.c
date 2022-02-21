#include <func.h>
//使用 udp 协议进行通信

int main(int argc, const char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        printf("./udp_client ip port\n");
        return -1;
    }
    // 1.socket
    int sockfd = -1;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM 代表使用 udp 通信协议
    ERROR_CHECK(sockfd, -1, "socket");

    // 2.收发数据
    struct sockaddr_in addr;                   //定义 sockaddr_in 结构体
    bzero(&addr, sizeof(addr));                //清空结构体初始化
    addr.sin_family = AF_INET;                 //使用 IPv4 通信协议
    addr.sin_port = htons(atoi(argv[2]));      //设置服务器端口
    addr.sin_addr.s_addr = inet_addr(argv[1]); //设置服务器 IP

    //客户端发送数据
    char buf[1024] = {0}; //定义缓冲区
    strcpy(buf, "This is client. Hello, server.");
    ret = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&addr, sizeof(addr));
    ERROR_CHECK(ret, -1, "sendto");

    // 3.收发数据
    //服务器先接收数据

    // struct sockaddr_in server_addr;           //定义 sockaddr_in 结构体
    // bzero(&server_addr, sizeof(server_addr)); //清空结构体初始化
    // socklen_t server_addrlen = 0;
    // ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr, &server_addrlen);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    ERROR_CHECK(ret, -1, "recvfrom");
    printf("recv msg:\n%s\n", buf);

    close(sockfd);
    return 0;
}