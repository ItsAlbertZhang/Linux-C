#include "queue.h"

int queue_in(struct queue_t *Q, int elem) {
    if (Q->front == Q->rear && Q->tag != 0) {
        return -1;
    } else {
        Q->p_q[Q->rear] = elem;
        Q->rear = (Q->rear + 1) % Q->size;
        Q->tag = 1;
    }
    return 0;
}

int queue_out(struct queue_t *Q, int *elem) {
    if (Q->front == Q->rear && Q->tag == 0) {
        return -1;
    } else {
        *elem = Q->p_q[Q->front];
        Q->front = (Q->front + 1) % Q->size;
        Q->tag = 0;
    }
    return 0;
}