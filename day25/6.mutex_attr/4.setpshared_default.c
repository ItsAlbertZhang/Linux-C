#include <func.h>
//未对锁的 pshared 属性进行修改时, 锁无法在进程间共享.

#define NUM 10000000 // 1 million
typedef struct _share_data_t {
    int result;            //结果
    pthread_mutex_t mutex; //线程锁
} data_t;                  //定义一个结构体, 用于进程之间传递数据

int main(int argc, const char *argv[]) {
    time_t startTime = time(NULL);
    int ret = 0;
    int shmid = shmget(1000, sizeof(data_t), 0600 | IPC_CREAT); //获取一块共享内存
    ERROR_CHECK(shmid, -1, "shmget");
    data_t *data = shmat(shmid, NULL, 0); //共享内存连接到本进程
    ERROR_CHECK(data, (void *)-1, "shmat");
    bzero(data, sizeof(data_t)); //清空共享内存
    pthread_mutexattr_t mutex;   //定义一个锁属性
    bzero(&mutex, sizeof(mutex));
    pthread_mutexattr_init(&mutex);                               //初始化锁属性
    pthread_mutexattr_setpshared(&mutex, PTHREAD_PROCESS_SHARED); //设置锁进程间共享
    pthread_mutex_init(&(data->mutex), &mutex);                   //初始化共享内存中的锁
    if (fork()) {
        //父进程
        for (int i = 0; i < NUM; i++) {
            pthread_mutex_lock(&(data->mutex)); //加锁
            (data->result)++;                   //自加
            pthread_mutex_unlock(&(data->mutex)); //解锁
        }
        wait(NULL); //等待子进程
        time_t endTime = time(NULL);
        printf("result = %d, 用时 %ld 秒.\n", data->result, endTime - startTime);
        shmdt(data);//解除共享内存连接
        ret = shmctl(shmid, IPC_RMID, NULL); //销毁共享内存
        ERROR_CHECK(ret, -1, "shmctl");
    } else {
        //子进程
        for (int i = 0; i < NUM; i++) {
            pthread_mutex_lock(&(data->mutex));   //加锁
            (data->result)++;                     //自加
            pthread_mutex_unlock(&(data->mutex)); //解锁
        }
        shmdt(data); //解除共享内存连接
    }
    return 0;
}