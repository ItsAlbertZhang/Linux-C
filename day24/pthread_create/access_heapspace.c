#include <func.h>
// 所有线程均可访问进程的堆空间

void *pthreadfunc(void *arg) {     //注册一个子线程函数
    char *p = (char *)arg;         //接收参数
    printf("child get = %s\n", p); //读取
    strcpy(p + 6, "thread");       //写入
}

int main(int argc, char const *argv[]) {
    int ret = 0;
    char *p = (char *)malloc(sizeof(char) * 128); //申请一块堆空间
    strcpy(p, "hello world");                     //向申请到的堆空间内填入字符
    //创建子线程
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, (void *)p);
    THREAD_ERR_CHECK(ret, "pthread_create");
    sleep(1);                     //睡一下, 等子线程执行完毕.
    printf("main get = %s\n", p); //读取子线程改变后的值
    return 0;
}