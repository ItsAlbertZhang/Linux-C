#include <func.h>

int main(int argc, char const *argv[]) {
    pid_t ret = fork();
    if (0 == ret) {
        printf("This is child process. My pid=%d, ppid=%d, pgid=%d.\n",
               getpid(), getppid(), getpgid(0));
        sleep(60);
    } else {
        printf("This is child process. My pid=%d, ppid=%d, pgid=%d.\n",
               getpid(), getppid(), getpgid(0));
        sleep(61);
    }
    return 0;
}