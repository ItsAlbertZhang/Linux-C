#include <func.h>
// sa_flags 没有设定 SA_NODEFER 时, 信号重定向函数执行时会遵循以下默认规则:
/*
    1. 不同的信号之间可以相互打断. 例如, 在 a 休眠期间传入 b 信号, 则系统会立即转而去处理更新的 b 信号.
    2. 相同的信号无法打断, 仅会被记录, 并于当前信号处理结束后执行. 例如, 在 a 休眠期间再次传入一个 a 信号, 则新的 a 信号不会被立即执行, 而是在当前的 a 信号结束后再执行.
    3. 信号被记录的最大数量是 1. 其原理在于相同的信号只会被 [信号队列](#信号队列) 接收并记录一次.
    例如, 在 a 休眠期间再次传入多个 a 信号, 则仅有一个信号会被记录并在当前的 a 信号结束后执行.
    4. 当记录队列中有多个记录时, 按照以上三条规则进行处理.
    例如, 在 a1 休眠期间传入一个 b1 信号, 之后在 b1 休眠的期间再次传入一个 a2 信号和一个 b2 信号.
    则无论传入 a2 和 b2 的顺序如何, b2 都会比 a2 更先执行.
    原因在于当 b1 结束后, 此时 b1 已退栈, 而 a1 尚未退栈.
    则传入的 a2 信号会由于 a1 尚未退栈而被阻塞, 此时b2会先执行.
*/

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
    act.sa_flags = SA_SIGINFO;      //设定信号处理为 sigaction 类型
    act.sa_sigaction = sigfunc;     //设定信号处理重定向目标为 sigfunc 函数
    sigaction(SIGINT, &act, NULL);  //将 SIGINT 信号重定向
    sigaction(SIGQUIT, &act, NULL); //将 SIGQUIT 信号重定向
    while (1)
        ; //死循环, 等待接收信号.
    return 0;
}