#include <func.h>

//使用 cancel 函数向子进程发送 cancel 信号, 当子进程处于非阻塞状态时 cancel 信号无法生效

void *pthreadfunc(void *arg) {
    printf("here1.\n");
    while (1)
        ;
    return NULL;
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL);
    THREAD_ERROR_CHECK(ret, "pthread_create");
    sleep(1);
    ret = pthread_cancel(pthid); //向子进程发送 cancel 信号
    THREAD_ERROR_CHECK(ret, "pthread_cancel");
    ret = pthread_join(pthid, NULL); //等待子进程
    return 0;
}