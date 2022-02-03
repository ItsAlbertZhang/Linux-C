#include <func.h>
//忽略信号. 信号不是被阻塞了, 而是切切实实地被处理并忽略了.
int main(int argc, char const *argv[]) {
    sig_t ret;
    ret = signal(SIGINT, SIG_IGN); //将 SIGINT 信号重定向至 SIG_IGN.
    if (ret == SIG_ERR) {
        return -1;
    }
    sleep(10);                     //睡觉 10 秒
    ret = signal(SIGINT, SIG_DFL); //将 SIGINT 信号重定向至默认处理
    if (ret == SIG_ERR) {
        return -1;
    }
    printf("开始接收信号.\n");
    while (1)
        ; //死循环, 等待信号传入
    return 0;
}