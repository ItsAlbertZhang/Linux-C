#include <func.h>
//多线程条件变量的定义, 初始化与等待激发.

typedef struct {
    pthread_mutex_t mutex; //锁
    pthread_cond_t cond;   //条件变量
} data_t;

void *pthreadfunc(void *arg) {
    int ret = 0;
    data_t *data = (data_t *)arg;
    struct timespec waittime;
    waittime.tv_nsec = 0;
    time_t now;
    printf("子线程启动完毕, 开始工作.\n");
    while (1) {
        waittime.tv_sec = time(NULL) + 5;
        pthread_mutex_lock(&data->mutex);                                   // lock
        ret = pthread_cond_timedwait(&data->cond, &data->mutex, &waittime); //等待, 线程于此处阻塞
        pthread_mutex_unlock(&data->mutex);                                 // unlock
        now = time(NULL);
        printf("%s 子线程已接收到信号. wait 函数的返回值为 %d.\n", ctime(&now), ret);
    }
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    data_t *data = (data_t *)malloc(sizeof(data_t));
    bzero(data, sizeof(data_t));
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->cond, NULL);
    ret = pthread_create(&pthid, NULL, pthreadfunc, (void *)data);
    THREAD_ERROR_CHECK(ret, "pthread_create");
    sleep(3);
    printf("主线程开始执行, 每3秒发送一次信号.\n");
    while (1) {
        sleep(3);
        pthread_cond_signal(&data->cond);
    }
    return 0;
}