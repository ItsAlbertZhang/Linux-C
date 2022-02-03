#include <func.h>
//私有 shmid
int main(int argc, char const *argv[]) {
    int ret = 0;
    int shmid = shmget(IPC_PRIVATE, 1024, 0600 | IPC_CREAT);
    ERROR_CHECK(ret, -1, "shmget");
    if (fork()) {                                //父进程
        char *p = (char *)shmat(shmid, NULL, 0); //映射
        ERROR_CHECK(p, (char *)-1, "shmat");
        strcpy(p, "hello, world"); //写入
        wait(NULL);                //等待子进程
        ret = shmdt(p);            //解除映射
        ERROR_CHECK(ret, -1, "shmdt");
        sleep(10);
        shmctl(shmid, IPC_RMID, NULL); //删除共享内存
    } else {
        char *p = (char *)shmat(shmid, NULL, 0); //映射
        ERROR_CHECK(p, (char *)-1, "shmat");
        printf("child get = %s\n", p); //读取
        ret = shmdt(p);                //解除映射
        ERROR_CHECK(ret, -1, "shmdt");
    }
    return 0;
}