#include "pthread_pool.h"

void *child_handle(void *args) {
    struct pthread_data_t *p_pthread_data = (struct pthread_data_t *)args;
    printf("child thread %ld start working.\n", p_pthread_data->pthid);

    int ret = 0;
    int connect_fd = -1;
    // 先上锁再进循环
    pthread_mutex_lock(&p_pthread_data->public_data->mutex);
    while (1) {
        // 尝试出队, 注意此时锁处于上锁状态
        ret = queue_out(p_pthread_data->public_data->queue, &connect_fd);
        if (-1 == ret || -1 == connect_fd) {
            // 出队失败, 此时队列为空, 则在条件变量上等待
            pthread_cond_wait(&p_pthread_data->public_data->cond, &p_pthread_data->public_data->mutex);
        } else {
            // 出队成功, 拿到传输的对端所连接的 socket 文件描述符
            // 首先执行解锁操作
            pthread_mutex_unlock(&p_pthread_data->public_data->mutex);
            printf("子线程 %ld 已唤醒, 开始执行任务.\n", p_pthread_data->pthid);
            // 执行文件传输
            // transcode
            sleep(10);
            close(connect_fd);
            connect_fd = -1;
            printf("子线程 %ld 任务已完成.\n", p_pthread_data->pthid);
            //上锁, 并继续循环尝试出队.
            pthread_mutex_lock(&p_pthread_data->public_data->mutex);
        }
        // 注意: 尝试出队失败则 wait, 尝试出队成功则执行任务. 只有在 wait 的上下半之间(阻塞过程中), 以及执行任务期间, 锁处于解锁状态, 其他时间锁一律处于上锁状态.
    }
}