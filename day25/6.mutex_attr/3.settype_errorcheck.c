#include <func.h>
//检测锁, 当对一个已上锁的锁再次进行加锁时, 会返回错误.

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_mutexattr_t attr;
    bzero(&attr, sizeof(attr));          //定义一个锁属性
    ret = pthread_mutexattr_init(&attr); //初始化锁属性
    THREAD_ERROR_CHECK(ret, "pthread_mutexattr_init");
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK); //设置锁属性为检测锁
    THREAD_ERROR_CHECK(ret, "pthread_mutexattr_settype");
    pthread_mutex_t mutex;
    bzero(&mutex, sizeof(mutex));            //定义一把锁
    ret = pthread_mutex_init(&mutex, &attr); //用 attr 锁属性初始化锁
    THREAD_ERROR_CHECK(ret, "pthread_mutex_init");
    ret = pthread_mutex_lock(&mutex); //加锁一次
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");
    printf("第一次加锁成功.\n");
    ret = pthread_mutex_lock(&mutex); //加锁两次
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");
    printf("第二次加锁成功.\n");
    return 0;
}