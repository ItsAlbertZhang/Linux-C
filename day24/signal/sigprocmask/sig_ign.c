#include <func.h>
//忽略是处理的一种, 并且在不阻塞的情况下会被立即处理.
//与 sigprocmask_ignore.c 对比理解.

int main(int argc, char const *argv[]) {
    int ret = 0;
    signal(SIGINT, SIG_IGN); //将 SIGINT 信号重定向
    sleep(5);                //睡觉 5 秒, 在此时输入 SIGINT 信号
    sigset_t mask;           //定义一个 sigset_t 信号集合
    sigemptyset(&mask);      //清空信号集合
    sigpending(&mask);       //将被阻塞的信号拿出 (其实没有信号被阻塞)
    if (sigismember(&mask, SIGINT)) {
        printf("存在 SIGINT 信号阻塞.\n"); //不会输出此语句
    } else {
        printf("无 SIGINT 信号阻塞.\n"); //会输出此语句
    }
    return 0;
}