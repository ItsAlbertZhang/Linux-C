#include <func.h>

int main(int argc, char const *argv[]) {
    pid_t childpid = fork();
    if (0 == childpid) {
        printf("This is child process.\n");
        sleep(1);
        // int i = 1 / 0; //用这行代码会让父进程走到 Child exit unusually.
        exit(10); //退出并返回 10
    } else {
        int status = 0;
        // pid_t ret = waitpid(childpid, &status, WNOHANG); //用这行代码大概率走到 No child exited.
        pid_t ret = waitpid(childpid, &status, 0);
        if (ret > 0) {
            if (WIFEXITED(status)) {
                printf("Child exit successfully with exit code %d.\n", WEXITSTATUS(status));
            } else {
                printf("Child exit unusually.\n");
            }
        } else {
            printf("No child exited.\n");
        }
    }
    return 0;
}