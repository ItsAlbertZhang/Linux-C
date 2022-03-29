#include "func.h"

struct config_t {
    char ip[16];
    char port[8];
    char filename[128];
};

struct trans_t {
    off_t buflen;
    char buf[1024];
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

// 循环接收
int recv_n(int sockfd, void *buf, size_t len, int flags) {
    int ret = 0;

    char *p = (char *)buf;
    while (len > 0) {
        ret = recv(sockfd, p, len, flags);
        ERROR_CHECK(ret, -1, "recv");
        p += ret;
        len -= ret;
    }
    // usleep(100000);// 测试使用, 避免文件传输太快
    return 0;
}

int sigalrmflag = 0;

void sigalrmfunc(int signum) { // 当接收到信号时, 将全局变量置 1.
    sigalrmflag = 1;
    return;
}

void print_progress_bar(int percent, int thousandpercent, int download_speed, int klevel) {
    int x = percent >> 2;
    int i = 0;
    printf("\r");
    printf("[");
    for(; i < x; i++) {
        printf("=");
    }
    for(; i < 25; i++) {
        printf(" ");
    }
    printf("]");
    printf("%3d.%1d%%  %4d ", percent, thousandpercent, download_speed);
    if (klevel == 1) {
        printf("K");
    } else if (klevel == 2) {
        printf("M");
    } else if (klevel == 3) {
        printf("G");
    } else if (klevel == 4) {
        printf("T");
    }
    printf("B/s");
    fflush(stdout);
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

    // 1.socket
    int connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(ret, -1, "socket");
    // 2.connect
    struct sockaddr_in server_addr;                                                  // 定义 sockaddr_in 结构体
    bzero(&server_addr, sizeof(server_addr));                                        // 清空结构体初始化
    server_addr.sin_family = AF_INET;                                                // 使用 IPv4 通信协议
    server_addr.sin_port = htons(atoi(conf.port));                                   // 设置服务器端口
    server_addr.sin_addr.s_addr = inet_addr(conf.ip);                                // 设置服务器 IP
    ret = connect(connect_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 连接服务器
    ERROR_CHECK(ret, -1, "connect");

    struct trans_t tr;
    printf("conf.filename = %s\n", conf.filename);
    int filefd = open(conf.filename, O_CREAT | O_TRUNC | O_RDWR, 0666);
    ERROR_CHECK(filefd, -1, "open");
    printf("filefd = %d\n", filefd);

    // 发送想要下载的文件的名称
    ret = send(connect_fd, conf.filename, strlen(conf.filename), 0);
    ERROR_CHECK(ret, -1, "send");

    // 接收文件大小
    off_t filesize = 0;
    ret = recv(connect_fd, &filesize, sizeof(filesize), 0);
    ERROR_CHECK(ret, -1, "recv");

    if (0 == filesize) { // 服务器上没有该文件, 接收的文件大小为 0
        close(filefd);
        ret = unlink(conf.filename);
        ERROR_CHECK(ret, -1, "unlink");
        printf("No such file exists on the server.\n");
    } else { //服务器上存在该文件

        // 进度显示功能--下载进度
        off_t persize = filesize / 1000; // 文件大小的千分之一为一份 (即 0.1%)
        if (!persize) {
            persize = 1; // 文件过小时 (小于 1KB), 以 1 字节为 0.1%
        }
        off_t nowsize = 0;       // 当前份的文件大小, 当其超过文件大小的千分之一时进行进位
        int percent = 0;         // 当前百分比, 其应小于 100
        int thousandpercent = 0; // 当前千分比, 其应小于 10, 当其等于 10 时进行进位
        // 进度显示功能--下载速度
        signal(SIGALRM, sigalrmfunc);                   //将 SIGALRM 信号重定向至 sigalrmfunc 函数. 该信号是真实计时器发出的信号
        struct itimerval persecond = {{1, 0}, {1, 0}};  //定义一个计时器时间设置, 间隔时间为 1 秒
        ret = setitimer(ITIMER_REAL, &persecond, NULL); //设定计时器
        ERROR_CHECK(ret, -1, "setitimer");
        off_t lastsecondrecvsize = 0; // 上一秒接收的文件大小
        extern int sigalrmflag;       // 计时器的脉冲信号会触发 SIGALRM 信号, 该信号被重定向至 sigalrmfunc 函数, 该函数内会将该全局变量置 1
        int klevel = 0;               // 实际文件大小为显示大小 download_speed 的 1024 的 klevel 次方字节
        off_t download_speed = 0;     // 显示大小 download_speed
        // 进度显示功能--总速度
        struct timeval starttime;
        bzero(&starttime, sizeof(starttime));
        ret = gettimeofday(&starttime, NULL);
        ERROR_CHECK(ret, -1, "gettimeofday");
        while (1) {
            // 先接收结构体的头部
            tr.buflen = 0;
            ret = recv(connect_fd, &tr.buflen, sizeof(tr.buflen), 0);
            ERROR_CHECK(ret, -1, "recv");
            if (0 == tr.buflen) {
                // 服务器发送完毕, 接收的结构体头部为 0
                close(filefd);
                break;
            } else {
                // 正常接收文件, 接收的结构体头部为接收内容大小
                ret = recv_n(connect_fd, &tr.buf, tr.buflen, 0); // 接收结构体的主体部分
                ERROR_CHECK(ret, -1, "recv");
                ret = write(filefd, tr.buf, tr.buflen); // 写入文件
                ERROR_CHECK(ret, -1, "write");
                // 进度显示功能--下载进度
                nowsize += tr.buflen;
                while (nowsize >= persize) {
                    thousandpercent++;
                    if (thousandpercent >= 10) {
                        percent++;
                        thousandpercent -= 10;
                    }
                    nowsize -= persize;
                }
                // 进度显示功能--下载速度
                lastsecondrecvsize += tr.buflen;
                if (sigalrmflag) { // 距离上次刷新下载速度已经过去 1 秒
                    klevel = -1;
                    do {
                        klevel++;
                        download_speed = lastsecondrecvsize;
                        lastsecondrecvsize >>= 10;
                    } while (lastsecondrecvsize);
                    sigalrmflag = 0;
                }
                print_progress_bar(percent, thousandpercent, download_speed, klevel);
            }
        }
        // 进度显示功能--总速度
        struct timeval endtime;
        bzero(&endtime, sizeof(endtime));
        ret = gettimeofday(&endtime, NULL);
        ERROR_CHECK(ret, -1, "gettimeofday");
        off_t download_speed_byte = filesize;
        download_speed_byte *= 1000000;
        download_speed_byte = download_speed_byte / ((endtime.tv_sec - starttime.tv_sec) * 1000000 + (endtime.tv_usec - starttime.tv_usec));
        klevel = -1;
        do {
            klevel++;
            download_speed = download_speed_byte;
            download_speed_byte >>= 10;
        } while (download_speed_byte);
        print_progress_bar(percent, thousandpercent, download_speed, klevel);
        printf("\n");
    }

    // 关闭连接
    close(connect_fd);
    return 0;
}