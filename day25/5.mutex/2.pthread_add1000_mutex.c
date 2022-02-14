#include <func.h>
//多线程操作, 使用到的技术为线程锁

#define NUM 10000000 // 1 million
typedef struct _share_data_t {
    int result;            //结果
    pthread_mutex_t mutex; //线程锁
} data_t;                  //定义一个结构体, 用于线程之间传递数据

void *pthreadfunc(void *arg) {
    data_t *data = (data_t *)arg;
    for (int i = 0; i < NUM; i++) {
        pthread_mutex_lock(&data->mutex);   //上锁
        (data->result)++;                   //自加
        pthread_mutex_unlock(&data->mutex); //解锁
    }
    return NULL;
}

int main(int argc, const char *argv[]) {
    time_t startTime = time(NULL);
    int ret = 0;
    pthread_t pthid;
    data_t data;                           //定义一个传参结构体
    bzero(&data, sizeof(data));            //清空结构体
    pthread_mutex_init(&data.mutex, NULL); //初始化锁, 锁属性为 NULL
    ret = pthread_create(&pthid, NULL, pthreadfunc, (void *)&data);
    THREAD_ERR_CHECK(ret, "pthread_create"); //创建子线程
    //主线程进行自加操作
    for (int i = 0; i < NUM; i++) {
        pthread_mutex_lock(&data.mutex);   //上锁
        (data.result)++;                   //自加
        pthread_mutex_unlock(&data.mutex); //解锁
    }
    pthread_join(pthid, NULL); //等待子线程
    time_t endTime = time(NULL);
    printf("result = %d, 总用时 %ld 秒.\n", data.result, endTime - startTime);
    return 0;
}