#include <func.h>

int main(int argc, char const *argv[]){
    ARGS_CHECK(argc, 3)
    int fdr, fdw;
    fdr = open(argv[1], O_RDONLY);
    ERROR_CHECK(fdr, -1, "open");
    fdw = open(argv[2], O_WRONLY);
    ERROR_CHECK(fdw, -1, "open");
    printf("I am chat 1\n");
    char buf[1024] = {0};
    while (1) {
        memset(buf, 0, sizeof(buf));
        read(STDIN_FILENO, buf, sizeof(buf));
        write(fdw, buf, strlen(buf) - 1);
        memset(buf, 0, sizeof(buf));
        int ret = read(fdr, buf, sizeof(buf));
        printf("%s\n", buf);
    }
    close(fdr);
    close(fdw);
    return 0;
}
