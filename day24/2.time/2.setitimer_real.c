#include <func.h>

void sigfunc(int signum) { //当接收到信号时, 打印一次当前时间.
    time_t now = time(NULL);
    printf("now time is %s", ctime(&now));
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    signal(SIGALRM, sigfunc); //将 SIGALRM 信号重定向至 sigfunc 函数. 该信号是真实计时器发出的信号.
    struct itimerval t = {{2, 0}, {3, 0}}; //定义一个计时器时间设置, 间隔时间为 2 秒, 初始启动时间为 3 秒.
    sigfunc(0);//先打印一次当前时间
    ret = setitimer(ITIMER_REAL, &t, NULL);//设定计时器
    ERROR_CHECK(ret, -1, "setitimer");
    //使用 read 系统调用阻塞进程
    char buf[1024] = {0};
    read(STDIN_FILENO, buf, sizeof(buf));
    printf("read get = %s", buf);
    return 0;
}