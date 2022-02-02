#include <func.h>

int main(int argc, char const *argv[]) {
    ARGS_CHECK(argc, 2)
    int fd = open(argv[1], O_RDWR);
    ERROR_CHECK(fd, -1, "open");
    char *p = mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sprintf(p, "world");
    munmap(p, 5);
    return 0;
}