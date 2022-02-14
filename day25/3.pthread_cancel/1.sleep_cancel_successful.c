#include <func.h>

//使用 cancel 函数向子进程发送 cancel 信号, 当子进程处于 sleep 状态时 cancel 信号会正常生效

void *pthreadfunc(void *arg) {
    printf("here1.\n");
    sleep(10);
    printf("here2.\n");
    return NULL;
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL);
    THREAD_ERR_CHECK(ret, "pthread_create");
    sleep(1);
    ret = pthread_cancel(pthid); //向子进程发送 cancel 信号
    THREAD_ERR_CHECK(ret, "pthread_cancel");
    ret = pthread_join(pthid, NULL); //等待子进程
    return 0;
}