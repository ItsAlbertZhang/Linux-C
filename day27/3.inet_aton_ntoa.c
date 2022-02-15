#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    struct in_addr addr;
    char buf[64] = {0};
    printf("Enter IP address:\n");
    scanf("%s", buf);
    ret = inet_aton(buf, &addr);
    if(ret != 1) {
        printf("Invalid IP address.\n");
        return -1;
    }
    printf("IP address: %08x\n", addr.s_addr);
    printf("IP address: %s\n", inet_ntoa(addr));
    printf("IP address: %08x\n", inet_addr(buf));
    return 0;
}