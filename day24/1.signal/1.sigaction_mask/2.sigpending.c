#include <func.h>

void sigfunc(int signum, siginfo_t *info, void *arg) {
    printf("sig %d is coming.\n", signum);
    sigset_t mask;
    bzero(&mask, sizeof(mask));
    printf("休眠 3 秒, 休眠结束后将检查是否有 SIGQUIT 信号阻塞.\n");
    sleep(3);
    sigpending(&mask);
    if (sigismember(&mask, SIGQUIT)) {
        printf("当前有 SIGQUIT 信号阻塞.\n");
    } else {
        printf("当前无 SIGQUIT 信号阻塞.\n");
    }
    printf("2 秒后本轮信号处理结束.\n");
    sleep(2);
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
    sigaction(SIGINT, &act, NULL);
    while (1)
        ;
    return 0;
}