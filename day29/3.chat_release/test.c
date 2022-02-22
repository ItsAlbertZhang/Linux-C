#include <func.h>

int main(int argc, const char *argv[]) {
    int ret = 0;
    printf("hello\n");
    sleep(1);
    printf("\033[s\033[1A\r");
    printf("world\nnihao\n");
    sleep(1);
    printf("\033[u");
    fflush(stdout);
    sleep(1);
    return 0;
}