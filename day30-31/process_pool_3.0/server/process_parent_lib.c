#include "process_parent.h"

int get_a_new_line(FILE *f, char *s, int len) {
    char *ret = NULL;

    ret = fgets(s, len, f);
    s[strlen(s) - 1] = 0;
    if (ret == NULL) {
        printf("fgets failed.\n");
        return -1;
    }

    return 0;
}

int getconfig(FILE *configfile, struct config_t *p_conf) {
    int ret = 0;

    ret = get_a_new_line(configfile, p_conf->ip, sizeof(p_conf->ip));
    ERROR_CHECK(ret, -1, "get_a_new_line");
    ret = get_a_new_line(configfile, p_conf->port, sizeof(p_conf->port));
    ERROR_CHECK(ret, -1, "get_a_new_line");
    ret = get_a_new_line(configfile, p_conf->child_process_number, sizeof(p_conf->child_process_number));
    ERROR_CHECK(ret, -1, "get_a_new_line");

    return 0;
}

int enqueue(struct queue_t *Q, int elem) {
    if (Q->front == Q->rear && Q->tag != 0) {
        return -1;
    } else {
        Q->p_q[Q->rear] = elem;
        Q->rear = (Q->rear + 1) % Q->size;
        Q->tag = 1;
    }
    return 0;
}

int dequeue(struct queue_t *Q, int *elem) {
    if (Q->front == Q->rear && Q->tag == 0) {
        return -1;
    } else {
        *elem = Q->p_q[Q->front];
        Q->front = (Q->front + 1) % Q->size;
        Q->tag = 0;
    }
    return 0;
}