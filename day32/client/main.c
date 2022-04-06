#include "lib_readconfig.h"
#include "lib_tcp_connect.h"
#include "pthread_pool.h"
#include "recv_file.h"

int main(int argc, char const *argv[]) {
    int ret = 0;

    // 读取配置文件
    char *conf = read_config_file("default.config");

    // 执行 tcp 初始化
    int connect_fd = tcp_connect(conf + 0 * MAX_CONFIG_LENGTH, conf + 1 * MAX_CONFIG_LENGTH);

    // 接收文件
    ret = recv_file(connect_fd, conf + 2 * MAX_CONFIG_LENGTH);
    ERROR_CHECK(ret, -1, "recv_file");

    // 断开连接
    close(connect_fd);

    return 0;
}