#include <func.h>

int main(int argc, char const *argv[]) {
    int fd = open("hello", O_RDWR);
    char buf[1024] = {0};
    pid_t ret;
    ret = fork();
    if(0 == ret) {
        read(fd, buf, 5);
        printf("%s\n", buf);
    } else {
        usleep(5000);
        read(fd, buf, 5);
        printf("%s\n", buf);
        usleep(10000);
    }
    return 0;
}