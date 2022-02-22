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
        int fcntlret = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, fcntlret | O_NONBLOCK);
    }
    struct epoll_event event;
    event.events = EPOLLIN | cmd; //监控 fd 可读性, 边缘触发
    event.data.fd = fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    return ret;
}

int recv_msg(int sockfd, char *buf, int buflen, int tempfd) {
    int ret = 0;
    while (1) {
        bzero(buf, buflen);
        ret = recv(sockfd, buf, buflen, 0);
        if (-1 == ret) { //读到结束
            break;
        } else if (0 == ret) { //对方断开连接
            return 1;
        } else { //正常接收消息, 将消息读到临时文件中
            ret = write(tempfd, buf, ret);
            ERROR_CHECK(ret, -1, "write");
        }
    }
    return 0;
}

int send_msg(int sockfd, char *buf, int buflen, const char *argv[]) {
    int ret = 0;
    int sendusername = 1;
    while (1) {
        bzero(buf, buflen);
        ret = read(STDIN_FILENO, buf, buflen);
        if (-1 == ret) { //读到结束
            break;
        } else if (0 == ret) { //试图断开连接
            return 1;
        } else { //正常读取, 发送读取到的消息
            if (sendusername) {
                int ret = 0;
                ret = send(sockfd, argv[3], strlen(argv[3]), 0);
                ERROR_CHECK(ret, -1, "send");
                ret = send(sockfd, "\n", 1, 0);
                ERROR_CHECK(ret, -1, "send");
                sendusername = 0;
            }
            ret = send(sockfd, buf, ret, 0);
            ERROR_CHECK(ret, -1, "send");
        }
    }
    return 0;
}

int tempfd_init(int *p_tempfd) {
    int ret = 0;
    int tempfd = open("./chat_client_temp", O_RDWR | O_CREAT | O_TRUNC, 0666);
    char buf[] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"; // 20 个换行符
    ret = write(tempfd, buf, strlen(buf));
    ERROR_CHECK(ret, -1, "write");
    *p_tempfd = tempfd;
    return 0;
}

void print_from_currpos(int fd, off_t *p_currpos) {
    lseek(fd, *p_currpos, SEEK_SET); //将文件光标偏移到该输出的位置
    char buf[128] = {0};
    int readret = 0;
    while (readret = read(fd, buf, sizeof(buf))) {
        write(STDOUT_FILENO, buf, readret);
        bzero(buf, sizeof(buf));
    }
    write(STDOUT_FILENO, "\n", 1);
}

void print_init(int fd, off_t *p_currpos) {
    print_from_currpos(fd, p_currpos);
    time_t now = time(NULL);
    printf("%s", ctime(&now));
}

void print_flush(int fd, off_t *p_currpos) {
    printf("\033[s"); //保存当前 shell 光标位置
    fflush(stdout);
    printf("\033[%dA\r", 2 * (MAX_MESSAGE + 1)); //将 shell 光标移动至对话框顶
    fflush(stdout);
    for(int i = 0; i < 2 * MAX_MESSAGE; i++) {
        printf("\033[K\n");
    }
    printf("\033[%dA\r", 2 * MAX_MESSAGE);
    fflush(stdout);
    print_from_currpos(fd, p_currpos);
    printf("\033[u"); //恢复 shell 光标位置
    fflush(stdout);
}

int currposright2(int fd, off_t *p_currpos) {
    int cnt = 2;
    char buf = 0;
    lseek(fd, *p_currpos, SEEK_SET);
    while (cnt) {
        read(fd, &buf, 1);
        if (buf == '\n') {
            cnt--;
        }
    }
    *p_currpos = lseek(fd, 0, SEEK_CUR);
    return 0;
}

void print_aftersend_flush(void) {
    printf("\033[1A\r\033[K");
    fflush(stdout);
}

int main(int argc, const char *argv[]) {
    //程序自检
    int ret = 0;
    if (argc != 4) {
        printf("./chat_client ip port username\n");
        return -1;
    }

    int sockfd = socket_connect(argv); //执行 socket, connect
    ERROR_CHECK(ret, -1, "socket_connect");

    int tempfd = 0;
    ret = tempfd_init(&tempfd); //创建一个用于存放聊天记录的临时文件
    ERROR_CHECK(ret, -1, "tempfd_init");
    off_t currpos = 0; //一开始的文件光标偏移量为 0
    print_init(tempfd, &currpos);

    int epfd = epoll_create(1);             //创建 epoll 句柄
    ret = ep_add_et(epfd, sockfd, EPOLLET); //在 epoll 中注册 sockfd
    ERROR_CHECK(ret, -1, "ep_add_et");
    ret = ep_add_et(epfd, STDIN_FILENO, EPOLLET); //在 epoll 中注册标准输入
    ERROR_CHECK(ret, -1, "ep_add_et");

    struct epoll_event events[2]; //定义一个用于返回监听结果的数组
    bzero(events, sizeof(events));
    int ep_ready = 0; //满足条件的监听结果个数

    char buf[128];
    int PROGRAM_RUNNING = 1;
    while (PROGRAM_RUNNING) {
        ep_ready = epoll_wait(epfd, events, 2, -1);
        ERROR_CHECK(-1, ep_ready, "epoll_wait");
        for (int i = 0; i < ep_ready; i++) {
            if (events[i].data.fd == sockfd) { //有新消息
                ret = recv_msg(sockfd, buf, sizeof(buf), tempfd);
                ERROR_CHECK(ret, -1, "recv_msg");
                if (1 == ret) { //对方断开了连接
                    PROGRAM_RUNNING = 0;
                    printf("对方断开了连接.\n");
                    break;
                }
                currposright2(tempfd, &currpos);
                print_flush(tempfd, &currpos);
            }
            if (events[i].data.fd == STDIN_FILENO) { //欲发送消息
                ret = send_msg(sockfd, buf, sizeof(buf), argv);
                ERROR_CHECK(ret, -1, "recv_msg");
                if (1 == ret) { //试图断开连接
                    PROGRAM_RUNNING = 0;
                    printf("正在断开连接...\n");
                    break;
                }
                print_aftersend_flush();
            }
        }
    }

    return 0;
}