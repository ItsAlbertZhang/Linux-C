#include <func.h>

int main(int argc, char const *argv[]) {
    pid_t ret;
    ret = fork();
    if(0 == ret) {
        usleep(10000);
        printf("I am child process. My pid = %d, ppid = %d.\n", getpid(), getppid());
        exit(0);
    } else {
        printf("I am parent process. My pid = %d, ppid = %d, childid = %d.\n", getpid(), getppid(), ret);
        int status;
        ret = wait(&status);
        if(WIFEXITED(status)) {
            printf("The child's exit code is %d.\n", WEXITSTATUS(status));
        } else {
            printf("Unexpected child exit.");
        }
    }
    return 0;
}