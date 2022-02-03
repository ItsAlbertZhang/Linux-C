#include <func.h>
//使用 sigprocmask 阻塞信号

void sigfunc(int signum) {
    printf("signum: %d\n", signum);
    sleep(3);
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    signal(SIGINT, sigfunc);
    sigset_t mask;                       //定义一个 sigset_t 信号集合
    sigemptyset(&mask);                  //清空信号集合
    sigaddset(&mask, SIGINT);            //将 SIGINT 信号添加至信号集合中
    sigprocmask(SIG_BLOCK, &mask, NULL); //阻塞信号集合中的所有信号
    printf("关键代码开始.\n");
    sleep(5);
    printf("关键代码结束.\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL); //解除信号集合中所有信号的阻塞
    return 0;
}