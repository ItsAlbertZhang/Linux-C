#include <func.h>
//使用父子进程同时对同一块共享内存进行非原子操作, 结果会出现错误.
#define NUM 10000000 // 1 million

int main(int argc, char const *argv[]) {
    int ret = 0;
    int shmid = shmget(1000, sizeof(int), 0600 | IPC_CREAT);
    ERROR_CHECK(shmid, -1, "shmget");
    int *p = (int *)shmat(shmid, NULL, 0);
    bzero(p, sizeof(int));
    if (fork()) { //父进程
        for (int i = 0; i < NUM; i++) {
            (*p)++;
        }
        wait(NULL);
        printf("result = %d\n", *p);
        ret = shmctl(shmid, IPC_RMID, NULL); //关闭共享内存
    } else {
        for (int i = 0; i < NUM; i++) {
            (*p)++;
        }
    }
    return 0;
}