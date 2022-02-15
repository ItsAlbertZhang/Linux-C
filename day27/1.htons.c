#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    uint16_t netport, hostport = 0x1234;
    netport = htons(hostport);
    printf("netport = 0x%x\n", netport);
    return 0;
}