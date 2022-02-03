#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    ARGS_CHECK(argc, 2);
    ret = kill((0 - atoi(argv[1])), SIGINT); //给参数进程发送 SIGINT 信号
    ERROR_CHECK(ret, -1, "kill");
    return 0;
}