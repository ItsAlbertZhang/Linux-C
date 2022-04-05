#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__

#include "head.h"

struct pthread_data_t {
    pthread_t pthid;                       // pthid 为子线程 id (每个线程独有)
    struct public_resource_t *public_data; // data 为进程内共用数据 (所有线程公有)
};

struct public_resource_t {
    struct queue_t *queue; // queue 为客户端队列
    pthread_mutex_t mutex; // mutex 为队列锁, 解锁后方可执行队列操作
    pthread_cond_t cond;   // cond 为子线程等待的条件变量
};

struct queue_t {
    int *queue;
    int len;   // 队列长度
    int front; // front 为队头元素下标
    int rear;  // rear 为队尾元素下标 + 1
    char tag;  // tag 为 1 代表上一次为入队操作, 为 0 代表为出队操作.
};

// 子函数处理句柄
void *child_handle(void *args);

// 队列初始化
int queue_init(struct queue_t **pQ, int len);
// 入队
int queue_in(struct queue_t *Q, int elem);
// 出队
int queue_out(struct queue_t *Q, int *elem);

#endif