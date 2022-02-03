#include <func.h>
//父子进程同时 sleep.
int main(int argc, char const *argv[]) {
    int ret = 0;
    if (fork()) {
        sleep(1000);
    } else {
        if (fork()) {
            sleep(1000);
        } else {
            sleep(10);
            printf("已向整个进程组发送 SIGINT 信号.\n");
            ret = kill(0, SIGINT); //杀死自己所属的整个进程组
            ERROR_CHECK(ret, -1, "kill");
        }
    }
    return 0;
}