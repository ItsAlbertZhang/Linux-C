#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    int fds[2];
    pipe(fds);
    printf("%d %d\n", fds[0], fds[1]);
    if(fork()) {
        close(fds[1]);
        char buf[1024] = {0};
        while(1) {
            read(fds[0], buf, 1024);
            printf("get=%s\n", buf);
            bzero(buf, 1024);
        }
    } else {
        close(fds[0]);
        char buf[1024] = {0};
        while(1) {
            scanf("%s", buf);
            write(fds[1], buf, strlen(buf));
            bzero(buf, 1024);
        }
    }
    return 0;
}