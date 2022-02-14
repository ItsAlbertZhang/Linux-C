#include <func.h>
//在子线程中使用 malloc 申请一块内存, 并在主线程中使用 join 函数在其结束后接取返回值.

void *pthreadfunc(void *arg) { //注册一个子线程函数
    char *p = (char *)malloc(sizeof(char) * 20);
    strcpy(p, "hello, world");
    pthread_exit((void *)p);
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, NULL); //创建子线程
    char *p = NULL;
    pthread_join(pthid, (void **)&p); //接取返回值
    printf("%s\n", p);                //打印
    return 0;
}