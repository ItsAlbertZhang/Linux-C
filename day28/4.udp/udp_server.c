#include <func.h>
//使用 udp 协议进行通信

int main(int argc, const char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        printf("./udp_server ip port\n");
        return -1;
    }
    // 1.socket
    int sockfd = -1;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM 代表使用 udp 通信协议
    ERROR_CHECK(sockfd, -1, "socket");

    // 2.bind
    struct sockaddr_in addr;                   //定义 sockaddr_in 结构体
    bzero(&addr, sizeof(addr));                //清空结构体初始化
    addr.sin_family = AF_INET;                 //使用 IPv4 通信协议
    addr.sin_port = htons(atoi(argv[2]));      //设置服务器端口
    addr.sin_addr.s_addr = inet_addr(argv[1]); //设置服务器 IP

    ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)); //绑定

    // 3.收发数据
    //服务器先接收数据
    char buf[1024] = {0};                     //定义缓冲区
    struct sockaddr_in client_addr;           //定义 sockaddr_in 结构体
    bzero(&client_addr, sizeof(client_addr)); //清空结构体初始化
    socklen_t client_addrlen = sizeof(client_addr);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addrlen);
    ERROR_CHECK(ret, -1, "recvfrom");
    printf("client addr = %s, client port = %d, recv msg:\n%s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buf);
    //服务器发送数据
    strcpy(buf, "This is server. You are now connected.");
    ret = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    ERROR_CHECK(ret, -1, "sendto");

    close(sockfd);
    return 0;
}