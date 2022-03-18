#include <func.h>
//使用 destory 时, 若锁处于锁定状态, 则函数失败并返回 errorno.

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_mutex_t mutex;
    bzero(&mutex, sizeof(mutex));           //定义一个线程锁
    ret = pthread_mutex_init(&mutex, NULL); //初始化锁, 属性为 NULL
    THREAD_ERROR_CHECK(ret, "pthread_mutex_init");
    ret = pthread_mutex_lock(&mutex); //上锁
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");
    ret = pthread_mutex_destroy(&mutex); //使用 destroy 尝试销毁锁
    THREAD_ERROR_CHECK(ret, "pthread_mutex_destroy");
    return 0;
}