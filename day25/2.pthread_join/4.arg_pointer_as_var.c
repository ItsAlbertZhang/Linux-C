#include <func.h>
//将长整型变量作为指针进行参数传递

void *pthreadfunc(void *arg) { //注册一个子线程函数
    long ret = (long)arg;
    ret *= 2;
    pthread_exit((void *)ret);
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    long arg = 0;
    printf("请输入一个数: ");
    fflush(stdout);
    ret = scanf("%ld", &arg);
    ERROR_CHECK(ret, EOF, "scanf");
    ret = pthread_create(&pthid, NULL, pthreadfunc, (void *)arg); //创建子线程
    long pthread_return;
    pthread_join(pthid, (void **)&pthread_return);
    printf("返回的值为 %ld.\n", pthread_return);
    return 0;
}