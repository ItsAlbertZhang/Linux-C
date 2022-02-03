#include <func.h>

int main(int argc, char const *argv[]) {
    if(fork()) {
        printf("This is parent.\n");
        int status = 0;
        wait(&status);
        if(WIFEXITED(status)) {
            printf("Child exit successfully with code %d.\n", WEXITSTATUS(status));
        } else{
            printf("Child exit failed.\n");
        }
    } else {
        printf("This is child.\n");
        sleep(1);
        printf("print but no \\n.");
        abort();//abort 会让子进程异常退出, 打印 Child exit failed.
    }
    return 0;
}