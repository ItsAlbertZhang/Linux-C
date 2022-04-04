#include "func.h"

#define PRINT_PROGRESS_BAR 1

struct config_t {
    char ip[16];
    char port[8];
    char filename[128];
};

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
    ret = get_a_new_line(configfile, p_conf->filename, sizeof(p_conf->filename));
    ERROR_CHECK(ret, -1, "get_a_new_line");

    return 0;
}

int tcp_init(const char *server_ip, const char *port) {
    int ret = 0;
    // 1.socket
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr;                                                  // 定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                        // 清空结构体初始化
    server_addr.sin_family = AF_INET;                                                // 使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(port));                                        // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(server_ip);                              // 设置服务器 IP
    ret = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 连接服务器
    ERROR_CHECK(ret, -1, "connect");
    return connect_fd;
}

int sigalrmflag = 0;

void sigalrmfunc(int signum) { // 当接收到信号时, 将全局变量置 1.
    sigalrmflag = 1;
    return;
}

void print_progress_bar(int percent, int thousandpercent, int download_speed, int klevel) {
    int x = percent / 5;
    int i = 0;
    printf("\r[");
    for (; i < x; i++) {
        printf("=");
    }
    printf("%*s]%3d.%1d%%     %4d ", 20 - i, "", percent, thousandpercent, download_speed);
    if (klevel == 0) {
        printf(" B/s");
    } else if (klevel == 1) {
        printf("KB/s");
    } else if (klevel == 2) {
        printf("MB/s");
    } else if (klevel == 3) {
        printf("GB/s");
    } else if (klevel == 4) {
        printf("TB/s");
    }

    fflush(stdout);
}

// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;
    // 进度显示功能--下载进度
    size_t persize = len / 1000; // 文件大小的千分之一为一份 (即 0.1%)
    if (!persize) {
        persize = 1; // 文件过小时 (小于 1KB), 以 1 字节为 0.1%
    }
    size_t nowsize = 0;      // 当前份的文件大小, 当其超过文件大小的千分之一时进行进位
    int percent = 0;         // 当前百分比, 其应小于 100
    int thousandpercent = 0; // 当前千分比, 其应小于 10, 当其等于 10 时进行进位
    // 进度显示功能--下载速度
    struct itimerval persecond = {{1, 0}, {1, 0}}; //定义一个计时器时间设置, 间隔时间为 1 秒
    size_t lastsecondrecvsize = 0;                 // 上一秒接收的文件大小
    extern int sigalrmflag;                        // 计时器的脉冲信号会触发 SIGALRM 信号, 该信号被重定向至 sigalrmfunc 函数, 该函数内会将该全局变量置 1
    int klevel = 0;                                // 实际文件大小为显示大小 download_speed 的 1024 的 klevel 次方字节
    size_t download_speed = 0;                     // 显示大小 download_speed
    // 进度显示功能--总速度
    struct timeval starttime;
    bzero(&starttime, sizeof(starttime));
    ret = gettimeofday(&starttime, NULL);
    ERROR_CHECK(ret, -1, "gettimeofday");

    if (flags & PRINT_PROGRESS_BAR) {                   // 打印此次循环接收的进度
        signal(SIGALRM, sigalrmfunc);                   //将 SIGALRM 信号重定向至 sigalrmfunc 函数. 该信号是真实计时器发出的信号
        ret = setitimer(ITIMER_REAL, &persecond, NULL); //设定计时器
        ERROR_CHECK(ret, -1, "setitimer");
    }

    char *p = (char *)buf;
    int recvlen = 0; // 已接收的字节数
    while (recvlen < len) {
        ret = recv(sockfd, p + recvlen, len, 0);
        ERROR_CHECK(ret, -1, "recv");
        recvlen += ret;
        if (flags & PRINT_PROGRESS_BAR) {
            // 进度显示功能--下载进度
            nowsize += ret;
            while (nowsize >= persize) {
                thousandpercent++;
                if (thousandpercent >= 10) {
                    percent++;
                    thousandpercent -= 10;
                }
                nowsize -= persize;
            }
            // 进度显示功能--下载速度
            lastsecondrecvsize += ret;
            if (sigalrmflag) { // 距离上次刷新下载速度已经过去 1 秒
                klevel = -1;
                do {
                    klevel++;
                    download_speed = lastsecondrecvsize;
                    lastsecondrecvsize >>= 10;
                } while (download_speed > 10000);
                sigalrmflag = 0;
            }
            print_progress_bar(percent, thousandpercent, download_speed, klevel);
        }
    }
    if (flags & PRINT_PROGRESS_BAR) {
        // 进度显示功能--总速度
        struct timeval endtime;
        bzero(&endtime, sizeof(endtime));
        ret = gettimeofday(&endtime, NULL);
        ERROR_CHECK(ret, -1, "gettimeofday");
        size_t download_speed_byte = len;
        download_speed_byte *= 1000000;
        download_speed_byte = download_speed_byte / ((endtime.tv_sec - starttime.tv_sec) * 1000000 + (endtime.tv_usec - starttime.tv_usec));
        klevel = -1;
        do {
            klevel++;
            download_speed = download_speed_byte;
            download_speed_byte >>= 10;
        } while (download_speed > 10000);
        print_progress_bar(100, 0, download_speed, klevel);
        printf("\n");
    }
    // usleep(100000);// 测试使用, 避免文件传输太快
    return 0;
}

int main(int argc, const char *argv[]) {
    int ret = 0;

    // 读取配置文件
    FILE *configfile = fopen("default.config", "rb");
    struct config_t conf;
    bzero(&conf, sizeof(conf));
    ret = getconfig(configfile, &conf);
    ERROR_CHECK(ret, -1, "getconfig");
    fclose(configfile);

    int connect_fd = tcp_init(conf.ip, conf.port); // 进行 tcp 初始化

    printf("conf.filename = %s\n", conf.filename);
    int filefd = open(conf.filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
    ERROR_CHECK(filefd, -1, "open");
    printf("filefd = %d\n", filefd);

    // 发送想要下载的文件的名称
    ret = send(connect_fd, conf.filename, strlen(conf.filename), 0);
    ERROR_CHECK(ret, -1, "send");

    // 接收文件大小
    size_t filesize = 0;
    ret = recv_n(connect_fd, &filesize, sizeof(filesize), 0);
    ERROR_CHECK(ret, -1, "recv_n");

    if (0 == filesize) { // 服务器上没有该文件, 接收的文件大小为 0
        close(filefd);
        ret = unlink(conf.filename);
        ERROR_CHECK(ret, -1, "unlink");
        printf("No such file exists on the server.\n");
    } else { // 服务器上存在该文件
        ret = ftruncate(filefd, filesize);
        ERROR_CHECK(ret, -1, "ftruncate");
        char *filebuf = (char *)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, filefd, 0);
        ret = recv_n(connect_fd, filebuf, filesize, PRINT_PROGRESS_BAR);
        ERROR_CHECK(ret, -1, "recv_n");
        munmap(filebuf, filesize);
        close(filefd);
    }

    // 关闭连接
    close(connect_fd);
    return 0;
}