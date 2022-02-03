#include <func.h>

int main(int argc, char const *argv[]) {
    pid_t ret;
    ret = fork();
    if(0 == ret) {
        usleep(10000);
        printf("I am child process. My pid = %d, ppid = %d.\n", getpid(), getppid());
    } else {
        printf("I am parent process. My pid = %d, ppid = %d, childid = %d.\n", getpid(), getppid(), ret);
        while(1);
    }
    return 0;
}