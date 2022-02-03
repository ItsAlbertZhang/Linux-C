#include <func.h>
// sa_flags 没有设定 SA_RESTART 时, 信号重定向函数执行时会打断正在阻塞的系统调用, 并令系统调用返回错误.
void sigfunc(int signum, siginfo_t *info, void *arg) {
    printf("sig %d is coming.\n", signum);
    return;
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    struct sigaction act; //定义一个信号处理信息结构体
    bzero(&act, sizeof(struct sigaction));
    act.sa_flags = SA_SIGINFO;     //设定信号处理为 sigaction 类型
    act.sa_sigaction = sigfunc;    //设定信号处理重定向目标为 sigfunc 函数
    sigaction(SIGINT, &act, NULL); //将 SIGINT 信号重定向
    char buf[1024] = {0};
    ret = read(STDIN_FILENO, buf, sizeof(buf));
    printf("ret = %d, buf = #%s#.\n", ret, buf);
    return 0;
}