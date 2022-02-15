#include <func.h>
//注意: gethostbyaddr 为不可重入函数.
//该函数的实现本质上是查询 hosts 文件.
//使用 127.0.0.1 可以得到正确的结果.

int main(int argc, const char *argv[]) {
    int ret = 0;
    struct hostent *host = NULL;
    char buf[128] = {0};
    printf("Enter host IP: ");
    fflush(stdout);
    scanf("%s", buf);
    struct in_addr addr;
    bzero(&addr, sizeof(struct in_addr));
    addr.s_addr = inet_addr(buf);
    host = gethostbyaddr((void *)&addr, sizeof(struct in_addr), AF_INET);
    if (NULL == host) {
        printf("Hostname \"%s\" not found\n", buf);
        return -1;
    }
    printf("Host name: \"%s\"\n", host->h_name); //真实域名
    for (int i = 0; NULL != host->h_aliases[i]; i++) {
        printf("Host aliases: \"%s\"\n", host->h_aliases[i]); //别名
    }
    printf("Host addrtype: %d\n", host->h_addrtype); //主机 IP 地址类型
    printf("Host length: %d\n", host->h_length);     //主机 IP 地址字节长度
    char dst[128] = {0};
    for (int i = 0; NULL != host->h_addr_list[i]; i++) {
        bzero(&dst, sizeof(dst));
        inet_ntop(host->h_addrtype, host->h_addr_list[i], dst, sizeof(dst));
        puts(dst); //主机IP地址
    }
    return 0;
}