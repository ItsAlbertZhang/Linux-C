#include <func.h>

int main(int argc, char const *argv[]) {
    sleep(5);
    char buf[128] = {0};
    read(STDIN_FILENO, buf, 128);
    printf("%s\n", buf);
    return 0;
}