#include <func.h>
//父子进程同时 sleep.
int main(int argc, char const *argv[]) {
    int ret = 0;
    if (fork()) {
        sleep(1000);
    } else {
        sleep(1000);
            ;
    }
    return 0;
}