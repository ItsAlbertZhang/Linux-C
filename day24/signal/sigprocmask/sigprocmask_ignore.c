#include <func.h>
//阻塞的优先级高于忽略, 或者说忽略是处理的一种, 而阻塞是暂时不处理.
//与 sig_ign.c 对比理解.

int main(int argc, char const *argv[]) {
    int ret = 0;
    signal(SIGINT, SIG_IGN);
    sigset_t mask;                       //定义一个 sigset_t 信号集合
    sigemptyset(&mask);                  //清空信号集合
    sigaddset(&mask, SIGINT);            //将 SIGINT 信号添加至信号集合中
    sigprocmask(SIG_BLOCK, &mask, NULL); //阻塞信号集合中的所有信号
    printf("关键代码开始.\n");
    sleep(5);
    printf("关键代码结束.\n");
    sigemptyset(&mask); //清空信号集合
    sigpending(&mask);  //将被阻塞的信号拿出
    if (sigismember(&mask, SIGINT)) {
        printf("存在 SIGINT 信号阻塞.\n");
    } else {
        printf("无 SIGINT 信号阻塞.\n");
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL); //解除信号集合中所有信号的阻塞
    return 0;
}