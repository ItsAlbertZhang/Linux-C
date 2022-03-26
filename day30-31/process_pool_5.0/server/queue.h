#ifndef queue_h
#define queue_h

struct queue_t {
    int *p_q;
    int size;  // 队列大小
    int front; // front 为队头元素下标
    int rear;  // rear 为队尾元素下标 + 1
    char tag;  // tag 为 1 代表上一次为入队操作, 为 0 代表为出队操作.
};

// 入队
int queue_in(struct queue_t *Q, int elem);
// 出队
int queue_out(struct queue_t *Q, int *elem);

#endif