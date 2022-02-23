#include <func.h>
//多人在线聊天室客户端
//已知问题: 当一行的字数过多时, 会出现问题.
//待实现功能: 时间每秒刷新

#define MAX_MESSAGE 10

struct sockaddr_in args_to_sockaddr(const char *argv[]) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    return addr;
}

int socket_connect(const char *argv[]) {
    int ret = 0;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0); //执行 socket
    ERROR_CHECK(ret, -1, "socket");

    struct sockaddr_in server_addr = args_to_sockaddr(argv);                     //获取 connect 函数需要使用的 sockaddr_in 结构体
    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //执行 connect
    ERROR_CHECK(ret, -1, "connect");

    ret = send(sockfd, argv[3], strlen(argv[3]), 0);
    ERROR_CHECK(ret, -1, "send");

    return sockfd;
}

int ep_add_et(int epfd, int fd, int cmd) {
    int ret = 0;
    if (cmd) { //修改 fd 为非阻塞
        int flag = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    }
    struct epoll_event event;
    event.events = EPOLLIN | cmd; //监控 fd 可读性, 边缘触发
    event.data.fd = fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    return ret;
}

void print_input(const char *username, const int enter_protect) {
    printf("\n\n\n\n\n\n\n\n\n\n");
    printf("\033[10A");
    fflush(stdout);
    printf("\033[s%s    ", username);
    if (enter_protect > 0) {
        printf("你之前的键入可能已被新消息覆盖. 使用 Enter 以查看你的完整输入. p = %d", enter_protect);
    } else if (enter_protect == 0) {
        printf("使用 Enter 以发送消息.");
    }
    printf("\033[K\n    ");
    fflush(stdout);
    return;
}

void print_message(const char *buf) {
    // printf("\033[u\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n\033[K\n");
    printf("\033[u%s\033[s", buf);

    fflush(stdout);
    return;
}

int recv_msg(int sockfd, char *buf, int buflen, const char *username, int *p_enter_protect) {
    int ret = 0;
    while (1) {
        bzero(buf, buflen);
        ret = recv(sockfd, buf, buflen, 0);
        if (-1 == ret) { //读到结束
            break;
        } else if (0 == ret) { //对方断开连接
            return 1;
        } else { //正常接收消息, 将接收到的消息打印
            print_message(buf);
        }
    }
    print_input(username, *p_enter_protect);
    return 0;
}

int handle_message(char *buf, int buflen, int *p_enter_protect, int sockfd, const char *username) {
    *p_enter_protect += 1;
    recv_msg(sockfd, buf, buflen, username, p_enter_protect);
}

int recover_input(char *buf, int buflen, int *inputcache_pipe, const char *username, int *p_enter_protect) {
    // inputcache_pipe 成员: 0为管道0的读端, 1为管道0的写端, 2为管道1的读端, 3为管道1的写端
    int ret = 0, ctrlD = 0;
    printf("\033[u");
    fflush(stdout);
    int arg = -1;
    if(*p_enter_protect) {
        arg++;
    }
    print_input(username, arg);
    while (1) {
        bzero(buf, buflen);
        ret = read(STDIN_FILENO, buf, buflen);
        if (-1 == ret) {
            break;
        } else if (0 == ret) {
            ctrlD = 1;
        } else {
            ret = write(inputcache_pipe[1], buf, ret);
        }
    }
    while (1) {
        bzero(buf, buflen);
        ret = read(inputcache_pipe[0], buf, buflen);
        if (ret == -1) {
            break;
        } else {
            write(inputcache_pipe[3], buf, ret);
        }
    }
    while (!ctrlD) {
        bzero(buf, buflen);
        ret = read(inputcache_pipe[2], buf, buflen);
        if (ret == -1) {
            break;
        } else {
            if (*p_enter_protect && buf[ret - 1] == '\n') {
                ret--;
            }
            write(STDOUT_FILENO, buf, ret);
            fflush(stdout);
            if (!*p_enter_protect && buf[ret - 1] == '\n') {
                ret--;
            }
            write(inputcache_pipe[1], buf, ret);
        }
    }
    if (ctrlD) {
        close(inputcache_pipe[2]);
    }
    return 0;
}

int send_msg(char *buf, int buflen, int sockfd, int *inputcache_pipe, const char *username, int *p_enter_protect) {
    int ret = 0, cnt = 1;
    while (1) {
        bzero(buf, buflen);
        ret = read(inputcache_pipe[0], buf, buflen);
        if (-1 == ret) { //读到结束
            break;
        } else if (0 == ret) { //试图断开连接
            return 1;
        } else {
            //发送读取到的消息
            if (cnt) { //发送用户名
                int ret = 0;
                ret = send(sockfd, username, strlen(username), 0);
                ERROR_CHECK(ret, -1, "send");
                ret = send(sockfd, "\033[K\n    ", strlen("\033[K\n    "), 0);
                ERROR_CHECK(ret, -1, "send");
                cnt = 0;
            }
            ret = send(sockfd, buf, ret, 0);
            ERROR_CHECK(ret, -1, "send");
        }
    }
    if (cnt == 0) {
        ret = send(sockfd, "\033[K\n", strlen("\033[K\n"), 0);
        ERROR_CHECK(ret, -1, "send");
    }
    print_input(username, 0);
    return 0;
}

int handle_input(char *buf, int buflen, int *p_enter_protect, int *inputcache_pipe, int sockfd, const char *username) {
    int ret = 0;
    if (*p_enter_protect) {
        ret = recover_input(buf, buflen, inputcache_pipe, username, p_enter_protect);
        *p_enter_protect = 0;
    } else {
        ret = recover_input(buf, buflen, inputcache_pipe, username, p_enter_protect);
        ret = send_msg(buf, buflen, sockfd, inputcache_pipe, username, p_enter_protect);
    }
    return ret;
}

int main(int argc, const char *argv[]) {
    //程序自检
    int ret = 0;
    if (argc != 4) {
        printf("./chat_client ip port username\n");
        return -1;
    }
    const char *username = argv[3];

    int sockfd = socket_connect(argv); //执行 socket, connect
    ERROR_CHECK(ret, -1, "socket_connect");

    // for(int i = 0; i < 1000; i++) {
    //     printf("\n");
    // }
    // printf("\33[1000A");
    // fflush(stdout);

    // int tempfd = 0;
    // ret = tempfd_init(&tempfd); //创建一个用于存放聊天记录的临时文件
    // ERROR_CHECK(ret, -1, "tempfd_init");
    // off_t currpos = 0; //一开始的文件光标偏移量为 0
    // print_init(tempfd, &currpos);

    int enter_protect = 0; //两步确认
    print_input(username, 0);

    int flag = 0;
    int inputcache_pipe[2][2];
    pipe(inputcache_pipe[0]);
    flag = fcntl(inputcache_pipe[0][0], F_GETFL);
    fcntl(inputcache_pipe[0][0], F_SETFL, flag | O_NONBLOCK);
    pipe(inputcache_pipe[1]);
    flag = fcntl(inputcache_pipe[1][0], F_GETFL);
    fcntl(inputcache_pipe[1][0], F_SETFL, flag | O_NONBLOCK);

    int epfd = epoll_create(1);             //创建 epoll 句柄
    ret = ep_add_et(epfd, sockfd, EPOLLET); //在 epoll 中注册 sockfd
    ERROR_CHECK(ret, -1, "ep_add_et");
    ret = ep_add_et(epfd, STDIN_FILENO, EPOLLET); //在 epoll 中注册标准输入
    ERROR_CHECK(ret, -1, "ep_add_et");

    struct epoll_event events[2]; //定义一个用于返回监听结果的数组
    bzero(events, sizeof(events));
    int ep_ready = 0; //满足条件的监听结果个数

    char buf[128];
    int PROGRAM_RUNNING = 1; //程序运行标志
    while (PROGRAM_RUNNING) {
        ep_ready = epoll_wait(epfd, events, 2, -1);
        ERROR_CHECK(-1, ep_ready, "epoll_wait");
        for (int i = 0; i < ep_ready; i++) {
            if (events[i].data.fd == sockfd) { //有新消息
                ret = handle_message(buf, sizeof(buf), &enter_protect, sockfd, username);
                ERROR_CHECK(ret, -1, "recv_msg");
                if (1 == ret) { //对方断开了连接
                    PROGRAM_RUNNING = 0;
                    printf("对方断开了连接.\n");
                    break;
                }
            }
            if (events[i].data.fd == STDIN_FILENO) { //欲发送消息, 或欲刷新之前的输入
                ret = handle_input(buf, sizeof(buf), &enter_protect, (int *)inputcache_pipe, sockfd, username);
                ERROR_CHECK(ret, -1, "recv_msg");
                if (1 == ret) { //试图断开连接
                    PROGRAM_RUNNING = 0;
                    printf("正在断开连接...\n");
                    break;
                }
            }
        }
    }

    return 0;
}