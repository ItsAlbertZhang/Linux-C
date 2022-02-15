#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    uint16_t hostport, netport = 0x1234;
    hostport = ntohs(netport);
    printf("hostport = 0x%x\n", hostport);
    return 0;
}