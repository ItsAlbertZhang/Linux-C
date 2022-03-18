#include <func.h>
//使用join等待的情况下, 主线程会等待子线程结束

void *pthreadfunc(void *arg) { //注册一个子线程函数
    printf("here1.\n");
    sleep(3);
    printf("here2.\n");
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL); //创建子线程
    THREAD_ERROR_CHECK(ret, "pthread_create");
    printf("In main, here.\n");
    pthread_join(pthid, NULL);
    return 0;
}