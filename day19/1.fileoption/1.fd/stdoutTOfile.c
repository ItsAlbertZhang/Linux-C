#include <func.h>

int main(int argc, char const *argv[]) {
    ARGS_CHECK(argc, 2)
    int fd = open(argv[1], O_RDWR);
    ERROR_CHECK(fd, -1, "open")
    printf("\n");
    int fd1 = dup2(fd, STDOUT_FILENO);
    char buf;
    while(scanf("%c", &buf) != EOF) {
        printf("%c", buf);
    }
    return 0;
}
