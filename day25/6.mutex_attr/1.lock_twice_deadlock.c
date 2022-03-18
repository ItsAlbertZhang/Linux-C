#include <func.h>
//默认属性的锁只有上锁和解锁两种状态.
//如果试图对一把已上锁的锁进行加锁, 而对锁加锁的进程又迟迟未释放资源, 则有可能引起死锁.

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_mutex_t mutex;
    bzero(&mutex, sizeof(mutex));
    pthread_mutex_init(&mutex, NULL); //初始化锁, 属性为 NULL
    THREAD_ERROR_CHECK(ret, "pthread_mutex_init");
    pthread_mutex_lock(&mutex); //上锁
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");
    pthread_mutex_lock(&mutex); //再次上锁, 造成死锁
    printf("here.\n");
    return 0;
}