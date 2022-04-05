#include "lib_readconfig.h"
#include "lib_tcp_init.h"
#include "pthread_pool.h"

#define MAX_WAIT_CONNECT 5

int main(int argc, char const *argv[]) {
    int ret = 0;

    // 读取配置文件
    char *conf = read_config_file("default.config");

    // 初始化资源
    struct pthread_data_t *p_pthread_data = (struct pthread_data_t *)malloc(sizeof(struct pthread_data_t) * atoi(conf + 0 * MAX_CONFIG_LENGTH));
    struct public_resource_t *public_data = (struct public_resource_t *)malloc(sizeof(struct public_resource_t));
    queue_init(&(public_data->queue), MAX_WAIT_CONNECT);
    pthread_mutex_init(&public_data->mutex, NULL);
    pthread_cond_init(&public_data->cond, NULL);
    // 拉起子线程
    for (int i = 0; i < atoi(conf + 0 * MAX_CONFIG_LENGTH); i++) {
        p_pthread_data[i].public_data = public_data;
        ret = pthread_create(&p_pthread_data[i].pthid, NULL, child_handle, (void *)&p_pthread_data[i]);
        THREAD_ERROR_CHECK(ret, "pthread_create");
    }

    // 执行 tcp 初始化
    int sockfd = tcp_init(conf + 1 * MAX_CONFIG_LENGTH, conf + 2 * MAX_CONFIG_LENGTH);

    while (1) {
        // 接受连接并入队
        int ret = accept_enqueue(sockfd, public_data->queue);

        // 向子线程发出委派
        if (-1 != ret) {
            pthread_cond_signal(&public_data->cond);
            printf("已向子线程委派任务.\n");
        }
    }

    return 0;
}

int accept_enqueue(int sockfd, struct queue_t *Q) {
    int ret = 0;

    // accept
    int connect_fd = -1;
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t addrlen = sizeof(client_addr);
    connect_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen); // 于此处阻塞
    ERROR_CHECK(ret, -1, "accept");
    printf("已与 %s 建立连接.\n", inet_ntoa(client_addr.sin_addr));

    // enqueue
    ret = queue_in(Q, connect_fd);

    if (-1 == ret) {
        printf("服务器过于繁忙, 无法处理更多请求, 已主动断开该连接.\n");
        close(connect_fd);
        connect_fd = -1;
    }

    return connect_fd;
}
