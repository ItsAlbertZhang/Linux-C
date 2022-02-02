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
        _exit(0);//_exit 不会在退出之前清理缓冲区
    }
    return 0;
}