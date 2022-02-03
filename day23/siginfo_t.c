#include <func.h>
// sigaction 基础使用
void sigfunc(int signum, siginfo_t *info, void *arg) {
    printf("sig %d is coming.\n", signum);
    printf("send uid = %d, send pid = %d\n", info->si_uid, info->si_pid);
    return;
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    struct sigaction act; //定义一个信号处理信息结构体
    bzero(&act, sizeof(struct sigaction));
    act.sa_flags = SA_SIGINFO;     //设定信号处理为 sigaction 类型
    act.sa_sigaction = sigfunc;    //设定信号处理重定向目标为 sigfunc 函数
    sigaction(SIGINT, &act, NULL); //将 SIGINT 信号重定向
    while (1)
        ; //死循环, 等待接收信号.
    return 0;
}