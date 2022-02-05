#include <func.h>

int main(int argc, char const *argv[]) {
    ARGS_CHECK(argc, 3)
    int fdr, fdw;
    //打开管道
    fdw = open(argv[1], O_WRONLY);
    ERROR_CHECK(fdr, -1, "open");
    fdr = open(argv[2], O_RDONLY);
    ERROR_CHECK(fdw, -1, "open");
    char buf[1024] = {0};
    printf("This is chat 2.\n"); //就绪
    fd_set readset;
    int selectret, ret;
    // struct timeval timeout;
    while (1) {
        //重置select目标
        FD_ZERO(&readset);
        FD_SET(STDIN_FILENO, &readset);
        FD_SET(fdr, &readset);
        //重置timeout计时
        // memset(&timeout, 0, sizeof(struct timeval));
        // timeout.tv_sec = 3;
        // selectret = select(fdr + 1, &readset, NULL, NULL, &timeout);
        selectret = select(fdr + 1, &readset, NULL, NULL, NULL);
        // if(selectret > 0) {
        if (FD_ISSET(fdr, &readset)) { //管道可读
            //读管道
            memset(buf, 0, sizeof(buf));
            ret = read(fdr, buf, sizeof(buf));
            if (0 == ret) {
                printf("The other side is disconnected.\n");
                break;
            }
            printf("\nchat1:\n\t%s\n", buf);
        }
        if (FD_ISSET(STDIN_FILENO, &readset)) { //标准输入可读
            //写管道
            memset(buf, 0, sizeof(buf));
            ret = read(STDIN_FILENO, buf, sizeof(buf));
            if (0 == ret) {
                printf("You have been disconnected.\n");
                break;
            }
            write(fdw, buf, strlen(buf) - 1);
        }
        // } else {
        //     printf("Wating for the other side...\n");
        // }
    }
    close(fdr);
    close(fdw);
    return 0;
}