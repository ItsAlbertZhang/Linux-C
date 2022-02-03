#include <func.h>

int main(int argc, char const *argv[]) {
    pid_t ret;
    ret = fork();
    if(0 == ret) {
        // usleep(10000);
        printf("This is child process. My pid = %d, ppid = %d.\n", getpid(), getppid());
    } else {
        usleep(10000);
        printf("This is parent process. My pid = %d, ppid = %d, childid = %d.\n", getpid(), getppid(), ret);
    }
    return 0;
}