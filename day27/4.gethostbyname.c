#include <func.h>
//注意: gethostbyname 为不可重入函数

int main(int argc, const char *argv[]) {
    int ret = 0;
    struct hostent *host = NULL;
    char buf[128] = {0};
    printf("Enter hostname: ");
    fflush(stdout);
    scanf("%s", buf);
    host = gethostbyname(buf);
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
        puts(dst);
    }
    return 0;
}