#include <func.h>

int main(int argc, char const *argv[]) {
    printf("uid = %d, gid = %d\n", getuid(), getgid());
    printf("euid = %d, egid = %d\n", geteuid(), getegid());
    return 0;
}