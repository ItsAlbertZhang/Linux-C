#include <func.h>

void sigfunc(int signum) { //自定义一个函数, 用于信号重定向
    printf("sig %d is coming.\n", signum);
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    if (SIG_ERR == signal(SIGINT, sigfunc)) { //注册信号重定向至自定义的函数
        printf("signal failed.\n");
        return -1;
    }
    while (1)
        ; //死循环等待信号
    return 0;
}