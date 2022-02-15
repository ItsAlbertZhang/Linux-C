#include <func.h>

void func(char *funcname) {
    time_t now = time(NULL);
    char *p = ctime(&now);
    printf("收到来自 %s 函数的调用, 正在获取时间...\n", funcname);
    sleep(3);
    printf("对于来自 %s 函数的调用, 当前时间为 %s", funcname, p);
}

void sigfunc(int signum) {//注册一个信号处理函数
    printf("已接收到 %d 号信号, 将执行一次信号处理函数.\n", signum);
    time_t now = time(NULL);
    printf("当前时间: %s", ctime(&now));
    func("sigfunc");
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    signal(SIGINT, sigfunc);
    time_t now = time(NULL);
    printf("当前时间: %s", ctime(&now));
    func("main");
    return 0;
}