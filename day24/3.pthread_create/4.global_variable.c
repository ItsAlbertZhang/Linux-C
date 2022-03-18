#include <func.h>
//主线程和子线程访问全局变量

int gval = 10;

void *pthreadfunc(void *arg) { //注册一个子线程函数
    gval = 5;
    printf("gval has changed.\n");
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    printf("gval = %d\n", gval); //打印 gval 的值
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL); //创建一个子线程
    THREAD_ERROR_CHECK(ret, "pthread_create");
    usleep(1000);
    printf("gval = %d\n", gval); //打印 gval 的值
    return 0;
}