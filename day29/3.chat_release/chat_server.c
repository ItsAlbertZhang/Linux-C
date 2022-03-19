#include <func.h>
//多人在线聊天室服务端

#define CONNECTION_HASHNUM 23

struct sockaddr_in args_to_sockaddr(const char *argv[]) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    return addr;
}

int socket_bind_listen(const char *argv[]) {
    int ret = 0;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); //执行 socket
    ERROR_CHECK(sockfd, -1, "socket");

    struct sockaddr_in server_addr = args_to_sockaddr(argv); //获取 bind 函数需要使用的 sockaddr_in 结构体
    int reuseaddr = 1;                                       //设置端口重用
    ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    ERROR_CHECK(ret, -1, "setsockopt");
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //执行 bind
    ERROR_CHECK(ret, -1, "bind");

    ret = listen(sockfd, 10); //执行 listen
    ERROR_CHECK(ret, -1, "listen");

    return sockfd;
}

struct user_info {
    struct sockaddr_in addr; //用户的连接方式, IP 地址, 端口
    int connect_fd;          //连接使用的 socket 描述符
    char name[20];           //用户名
};

struct user_node {
    struct user_info *info;
    struct user_node *next;
};

struct user_node *hashtable_init(int hashnum) {
    //申请一块堆内存, 用于储存哈希表表头
    struct user_node *user_hashtable = (struct user_node *)malloc(sizeof(struct user_node) * hashnum);
    bzero(user_hashtable, sizeof(struct user_node) * hashnum);
    return user_hashtable;
}

int hashtable_add(struct user_node *user_ht, struct user_info *info) {
    int ret = 0;
    int i = info->connect_fd % CONNECTION_HASHNUM;
    struct user_node *lastnode = user_ht + i;
    while (lastnode->next) {
        lastnode = lastnode->next;
    }
    struct user_node *newnode = (struct user_node *)malloc(sizeof(struct user_node));
    newnode->info = info;
    newnode->next = NULL;
    lastnode->next = newnode;
    return 0;
}

int hashtable_del(struct user_node *user_ht, int fd, char *name) {
    int ret = -1;
    int i = fd % CONNECTION_HASHNUM;
    struct user_node *prenode = user_ht + i;
    while (prenode->next) {
        if (prenode->next->info->connect_fd == fd) {
            struct user_node *thisnode = prenode->next;
            prenode->next = thisnode->next;
            strcpy(name, thisnode->info->name);
            free(thisnode->info);
            free(thisnode);
            ret = 0;
            break;
        }
        prenode = prenode->next;
    }
    return ret;
}

int ep_add_et(int epfd, int fd, int cmd) {
    int ret = 0;
    if (cmd) { //修改 fd 为非阻塞
        int fcntlret = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, fcntlret | O_NONBLOCK);
    }
    struct epoll_event event;
    event.events = EPOLLIN | cmd; //监控 fd 可读性, 边缘触发
    event.data.fd = fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    return ret;
}

int ep_del(int epfd, int fd) {
    int ret = 0;
    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
    return ret;
}

int accept_connect(int sockfd, struct user_info **info_p) {
    int ret = 0;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addrlen = sizeof(addr);
    ret = accept(sockfd, (struct sockaddr *)&addr, &addrlen);
    ERROR_CHECK(ret, -1, "accept");
    struct user_info *info = (struct user_info *)malloc(sizeof(struct user_info)); //申请一块堆内存, 用于存储用户信息
    bzero(info, sizeof(struct user_info));
    memcpy(&info->addr, &addr, sizeof(addr));                         //用户的连接方式, IP 地址, 端口
    info->connect_fd = ret;                                           //连接使用的 socket 描述符
    ret = recv(info->connect_fd, &info->name, sizeof(info->name), 0); //用户名
    ERROR_CHECK(ret, -1, "recv");
    *info_p = info;
    return 0;
}

int disconnect(int fd) {
    int ret = 0;
    close(fd);
    return 0;
}

int broadcast_msg(struct user_node *user_ht, char *buf, int buflen, int recvfromfd) {
    int ret = 0;
    for (int i = 0; i < CONNECTION_HASHNUM; i++) {
        struct user_node *thisnode = user_ht + i;
        while (thisnode->next) {
            thisnode = thisnode->next;
            if (recvfromfd != thisnode->info->connect_fd) {
            ret = send(thisnode->info->connect_fd, buf, buflen, 0);
            ERROR_CHECK(ret, -1, "send");
            }
        }
    }
    return 0;
}

int recvsend_msg(int fd, struct user_node *user_ht) {
    int ret = 0;
    char buf[128];
    while (1) {
        bzero(buf, sizeof(buf));
        ret = recv(fd, buf, sizeof(buf), 0);
        if (-1 == ret) { //读到结束
            break;
        } else if (0 == ret) { //对方断开连接
            return 1;
        } else { //正常接收消息, 此时向群内所有成员广播(消息来源成员除外)
            ret = broadcast_msg(user_ht, buf, ret, fd);
            ERROR_CHECK(ret, -1, "broadcast_msg");
        }
    }
    return 0;
}

int main(int argc, const char *argv[]) {
    //程序自检
    int ret = 0;
    if (argc != 3) {
        printf("./chat_server ip port\n");
        return -1;
    }

    int sockfd = socket_bind_listen(argv); //执行 socket, bind, listen
    ERROR_CHECK(sockfd, -1, "socket_bind_listen");

    struct user_node *user_ht = hashtable_init(CONNECTION_HASHNUM); //初始化用于存储用户信息的哈希表

    int epfd = epoll_create(1);       //创建 epoll 句柄
    ret = ep_add_et(epfd, sockfd, 0); //在 epoll 中注册 sockfd
    ERROR_CHECK(ret, -1, "ep_add_et");
    ret = ep_add_et(epfd, STDIN_FILENO, 0); //在 epoll 中注册标准输入
    ERROR_CHECK(ret, -1, "ep_add_et");

    struct epoll_event events[CONNECTION_HASHNUM]; //定义一个用于返回监听结果的数组
    bzero(events, sizeof(events));
    int ep_ready = 0; //满足条件的监听结果个数

    int PROGRAM_RUNNING = 1;
    while (PROGRAM_RUNNING) {
        ep_ready = epoll_wait(epfd, events, CONNECTION_HASHNUM, -1);
        ERROR_CHECK(-1, ep_ready, "epoll_wait");
        for (int i = 0; i < ep_ready; i++) {
            if (events[i].data.fd == sockfd) { // sockfd 可读, 有新用户注册
                struct user_info *info = NULL;
                ret = accept_connect(sockfd, &info); //接受连接
                ERROR_CHECK(ret, -1, "accept_connect");
                printf("%s 加入了群聊.\n", info->name);
                ret = hashtable_add(user_ht, info); //将用户添加至哈希表
                ERROR_CHECK(ret, -1, "hashtable_add");
                ret = ep_add_et(epfd, info->connect_fd, EPOLLET); //将用户的通信 socket 描述符注册至 epoll 监听
                ERROR_CHECK(ret, -1, "ep_add_et");
            } else if (events[i].data.fd == STDIN_FILENO) {
                char buf[128] = {0};
                read(STDIN_FILENO, buf, sizeof(buf));
                if (0 == strcmp(buf, "exit\n")) {
                    PROGRAM_RUNNING = 0;
                    break;
                }
            } else { //有用户在群聊中发送消息
                ret = recvsend_msg(events[i].data.fd, user_ht);
                ERROR_CHECK(ret, -1, "recvsend_msg");
                if (1 == ret) { //对方断开了连接
                    char name[20] = {0};
                    ret = ep_del(epfd, events[i].data.fd); //将用户的通信 socket 描述符从 epoll 监听中删除
                    ERROR_CHECK(ret, -1, "ep_del");
                    ret = hashtable_del(user_ht, events[i].data.fd, name); //从哈希表中删除该用户
                    ERROR_CHECK(ret, -1, "hashtable_del");
                    ret = disconnect(events[i].data.fd); //关闭连接
                    ERROR_CHECK(ret, -1, "disconnect");
                    printf("%s 离开了群聊.\n", name);
                }
            }
        }
    }

    return 0;
}