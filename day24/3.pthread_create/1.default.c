#include <func.h>

void *pthreadfunc(void *arg) {//注册一个子线程函数
    printf("Here is child thread.\n");
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL);
    THREAD_ERR_CHECK(ret, "pthread_create");
    usleep(1000);//睡一下, 否则主线程有可能在子线程之前关闭.
    printf("Here is main thread.\n");
    return 0;
}