#include <func.h>
// sa_flags 设定 SA_NODEFER 时, 信号重定向函数执行时可以不断重入, 次数不丢失
void sigfunc(int signum, siginfo_t *info, void *arg) {
    printf("sig %d is coming.\n", signum);
    sleep(3); //睡觉 3 秒, 模拟信号重定向函数执行过程
    printf("sig %d over.\n", signum);
    return;
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    struct sigaction act; //定义一个信号处理信息结构体
    bzero(&act, sizeof(struct sigaction));
    act.sa_flags = SA_SIGINFO | SA_NODEFER; //设定信号处理为 sigaction 类型
    act.sa_sigaction = sigfunc;             //设定信号处理重定向目标为 sigfunc 函数
    sigaction(SIGINT, &act, NULL);          //将 SIGINT 信号重定向
    sigaction(SIGQUIT, &act, NULL);         //将 SIGQUIT 信号重定向
    while (1)
        ; //死循环, 等待接收信号.
    return 0;
}