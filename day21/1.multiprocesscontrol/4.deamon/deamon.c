#include <func.h>

int DeamonCreate(void) {
    if (fork()) {
        exit(0); //父进程退出
    }
    setsid();   //成为新进程组长和新会话领导, 脱离控制终端
    chdir("/"); //设置工作目录为根目录
    umask(0);   //重设文件访问掩码
    for (int i = 0; i < 64; i++) {
        close(i); //尽可能关闭从父进程继承来的文件
    }
}

int main(int argc, char const *argv[]) {
    DeamonCreate();
    while (1) {
        sleep(1);
    }
    return 0;
}