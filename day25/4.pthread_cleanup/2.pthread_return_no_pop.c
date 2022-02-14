#include <func.h>
//当程序运行到 pthread_cleanup_pop 时, 会执行清理函数

void my_cleanup_malloc(void *arg) {
    free(arg);
}

void my_cleanup_open(void *arg) {
    int fd = (int)(long)arg;
    close(fd);
}

int firstrun = 1;
int cnt = 0;

void *pthreadfunc(void *arg) {
    int fd = open("file", O_RDWR); //打开文件
    // pthread_cleanup_push(my_cleanup_open, (void *)(long)fd); //注册清理行为
    char *buf = (char *)malloc(sizeof(char) * 102400); //申请一块堆内存
    // pthread_cleanup_push(my_cleanup_malloc, (void *)buf);    //注册清理行为
    if (firstrun) {
        firstrun = 0;
        printf("打开文件的描述符编号为 %d.\n", fd);
        printf("子线程函数执行成功, 睡眠 3 秒后退出.\n");
        sleep(3);
    } else {
        printf("第 %d 次申请堆内存.\n", cnt++);
    }
    // pthread_cleanup_pop(1);
    // pthread_cleanup_pop(1);
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    pthread_create(&pthid, NULL, pthreadfunc, NULL);
    pthread_join(pthid, NULL); //等待子线程退出
    printf("在主线程中尝试打开文件:\n");
    int fd = open("file", O_RDWR);                   //打开文件
    char *buf = (char *)malloc(sizeof(char) * 1024); //申请一块堆内存
    printf("打开文件的描述符编号为 %d.\n", fd);
    printf("睡眠 3 秒后, 将不断执行子线程. 如果内存发生泄漏 (malloc 函数注册的清理行为未生效), 可以在资源监视器中看出.\n");
    sleep(3);
    while (1) {
        pthread_create(&pthid, NULL, pthreadfunc, NULL);
        pthread_join(pthid, NULL); //等待子线程退出
    }
    return 0;
}