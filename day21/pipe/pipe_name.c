#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    mkfifo("./pipe_cache", 0775);
    if (fork()) {
        int fd = open("pipe_cache", O_RDONLY);
        char buf[1024] = {0};
        for(int i = 0; i <10; i++) {
            read(fd, buf, 1024);
            printf("get=%s\n", buf);
            bzero(buf, 1024);
        }
        wait(NULL);
        unlink("./pipe_cache"); //解除链接(即删除管道文件)
    } else {
        int fd = open("pipe_cache", O_WRONLY);
        char buf[1024] = {0};
        for(int i = 0; i < 10; i++) {
            scanf("%s", buf);
            write(fd, buf, strlen(buf));
            bzero(buf, 1024);
        }
    }
    return 0;
}