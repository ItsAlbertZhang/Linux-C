#include <func.h>

void alarmfunc(int signum) {
    // do nothing
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    signal(SIGALRM, alarmfunc);
    alarm(3); // 3秒后向当前进程发送 SIGALRM 信号
    pause();
    return 0;
}