#include "main.h"
#include "getconfig.h"
#include "queue.h"
#include "process_child.h"
#include "process_parent.h"

int main(int argc, const char *argv[]) {
    int ret = 0;

    // 读取配置文件
    FILE *configfile = fopen("default.config", "rb");
    struct config_t conf;
    bzero(&conf, sizeof(conf));
    ret = getconfig(configfile, &conf);
    ERROR_CHECK(ret, -1, "getconfig");
    fclose(configfile);

    // 创建子进程
    int child_process_num = atoi(conf.child_process_number);
    struct process_data_t *p_process_ctl = (struct process_data_t *)malloc(sizeof(struct process_data_t) * child_process_num);
    ret = create_child_process(p_process_ctl, child_process_num);
    ERROR_CHECK(ret, -1, "create_child_process");

    // 创建一个就绪的循环队列
    struct queue_t queue;
    bzero(&queue, sizeof(queue));
    queue.size = child_process_num;
    queue.p_q = (int *)malloc(sizeof(int) * queue.size);
    for (int i = 0; i < child_process_num; i++) {
        ret = queue_in(&queue, i); // 入队
        ERROR_CHECK(ret, -1, "queue_in");
    }

    // 执行 socket, bind, listen
    int sockfd = tcp_init(conf.ip, conf.port);
    ERROR_CHECK(sockfd, -1, "tcp_init");
    printf("main: socket fd = %d.\n", sockfd);

    int epfd = epoll_create(1); // 创建 epoll 句柄
    printf("main: epoll fd = %d.\n", epfd);
    ret = epoll_add(epfd, sockfd); // 将 sockfd 添加至 epoll 监听
    ERROR_CHECK(ret, -1, "epoll_add");
    for (int i = 0; i < child_process_num; i++) {
        epoll_add(epfd, p_process_ctl[i].pipefd);
        ERROR_CHECK(ret, -1, "epoll_add");
    } // 将与子进程进行通信的管道添加至 epoll 监听

    struct epoll_event *events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * (child_process_num + 1));
    bzero(events, sizeof(struct epoll_event) * (child_process_num + 1)); // 定义一个返回的监听结果的数组

    int ep_ready = 0;        //满足条件的监听结果个数
    int PROGRAM_RUNNING = 1; // 程序继续运行标志
    while (PROGRAM_RUNNING) {
        ep_ready = epoll_wait(epfd, events, child_process_num + 1, -1);
        ERROR_CHECK(ep_ready, -1, "epoll_wait");
        for (int i = 0; i < ep_ready; i++) {
            if (events[i].data.fd == sockfd) { // sockfd 可读, 有新用户注册
                int process_ready = -1;
                ret = queue_out(&queue, &process_ready); // 出队
                if (ret != -1) {
                    ret = new_connect(sockfd, p_process_ctl + process_ready); // 建立连接
                    ERROR_CHECK(ret, -1, "new_connect");
                    ret = sendfd(p_process_ctl + process_ready);
                    ERROR_CHECK(ret, -1, "sendfd");
                }
            } else {
                int process_ready = -1;
                ret = read(events[i].data.fd, &process_ready, sizeof(process_ready));
                ERROR_CHECK(ret, -1, "read");
                ret = queue_in(&queue, process_ready); // 入队
                ERROR_CHECK(ret, -1, "queue_in");
                ret = close_connect(p_process_ctl + process_ready);
                ERROR_CHECK(ret, -1, "close_connect");
            }
        }
    }

    sleep(10000);

    return 0;
}

int child(const int pipe, const int id) {
    int ret = 0;
    int epfd = epoll_create(1);  // 创建 epoll 句柄
    ret = epoll_add(epfd, pipe); // 将与父进程通信的管道添加至 epoll 监听
    ERROR_CHECK(ret, -1, "epoll_add");
    struct epoll_event events;
    bzero(&events, sizeof(events));

    int connect_fd = 0;
    int PROGRAM_RUNNING = 1;
    while (PROGRAM_RUNNING) {
        ret = epoll_wait(epfd, &events, 1, -1);
        if (ret > 0) {
            ret = recvfd(pipe, &connect_fd);
            ERROR_CHECK(ret, -1, "recvfd");

            char filename[128] = {0};                              // 客户端请求文件名
            ret = recv(connect_fd, filename, sizeof(filename), 0); // 从客户端接收请求文件名
            ERROR_CHECK(ret, -1, "recv");
            printf("filename = %s\n", filename);

            ret = transfile(connect_fd, filename);
            ERROR_CHECK(ret, -1, "transfile");

            close(connect_fd);
            write(pipe, &id, sizeof(id)); // 告知父进程当前任务已完成
        }
    }
    return 0;
}