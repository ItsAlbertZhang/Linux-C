#include "lib_readconfig.h"
#include "lib_tcp_connect.h"
#include "pthread_pool.h"

int main(int argc, char const *argv[]) {
    int ret = 0;

    // 读取配置文件
    char *conf = read_config_file("default.config");

    // 执行 tcp 初始化
    int connect_fd = tcp_connect(conf + 0 * MAX_CONFIG_LENGTH, conf + 1 * MAX_CONFIG_LENGTH);

    char buf[1024] = {0};
    ret = recv(connect_fd, buf, sizeof(buf), 0);
    printf("ret = %d\n", ret);

    return 0;
}