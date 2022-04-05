#include "pthread_pool.h"

int queue_init(struct queue_t **pQ, int len) {
    *pQ = (struct queue_t *)malloc(sizeof(struct queue_t));
    struct queue_t *Q = *pQ;
    bzero(Q, sizeof(struct queue_t));
    Q->queue = (int *)malloc(sizeof(int) * len);
    bzero(Q->queue, sizeof(int) * len);
    Q->len = len;
    return 0;
}

int queue_in(struct queue_t *Q, int elem) {
    if (Q->front == Q->rear && Q->tag != 0) {
        // 队满
        return -1;
    } else {
        Q->queue[Q->rear] = elem;
        Q->rear = (Q->rear + 1) % Q->len;
        Q->tag = 1;
    }
    return 0;
}

int queue_out(struct queue_t *Q, int *elem) {
    if (Q->front == Q->rear && Q->tag == 0) {
        // 队空
        return -1;
    } else {
        *elem = Q->queue[Q->front];
        Q->front = (Q->front + 1) % Q->len;
        Q->tag = 0;
    }
    return 0;
}