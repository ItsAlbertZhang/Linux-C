#include <func.h>
//不使用join等待的情况下, 子线程会被强行结束

void *pthreadfunc(void *arg) { //注册一个子线程函数
    printf("here1.\n");
    sleep(3);
    printf("here2.\n");
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL);//创建子线程
    THREAD_ERR_CHECK(ret, "pthread_create");
    printf("In main, here.\n");
    return 0;
}