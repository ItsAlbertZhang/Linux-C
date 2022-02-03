#include <func.h>

void sigfunc(int signum, siginfo_t *info, void *arg) {
    printf("sig %d is coming.\n", signum);
    sleep(3); //睡觉 3 秒, 模拟信号重定向函数执行过程
    printf("sig %d over.\n", signum);
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    struct sigaction act;
    bzero(&act, sizeof(struct sigaction));
    act.sa_flags = SA_SIGINFO;  //设定信号处理为 sigaction 类型
    act.sa_sigaction = sigfunc; //设定信号处理重定向目标为 sigfunc 函数
    //对 sa_mask 进行操作
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT); //将 SIGQUIT 加入阻塞队列
    if (sigismember(&act.sa_mask, SIGQUIT)) {
        printf("SIGQUIT 信号阻塞设定成功.\n");
    }
    sigaction(SIGINT, &act, NULL); //将 SIGINT 信号重定向
    while (1)
        ;
    return 0;
}