#include <func.h>
//同时访问共享资源时出错, 可以通过批处理思想处理解决.
//分时思想需要使用后面学习的线程锁

//最终整体打印的结果是正确的.

//子线程函数中, 将接收到的 id 参数打印出来
void *pthreadfunc(void *arg) {
    printf("当前线程: %ld.\n", (long)arg);
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    for (long i = 0; i < 10; i++) { //创建10个进程, 并将进程 id 作为参数传递给进程
        pthread_create(&pthid, NULL, pthreadfunc, (void *)i);
        printf("线程 %ld 创建完毕.\n", i);
        pthread_join(pthid, NULL);//等待子进程结束再进行.
    }
    usleep(5000);
    return 0;
}