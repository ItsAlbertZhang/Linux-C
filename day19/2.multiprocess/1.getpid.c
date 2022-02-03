#include <func.h>

int main(int argc, char const *argv[]) {
    printf("pid = %d, ppid = %d\n", getpid(), getppid());
    return 0;
}
