#include <func.h>
//多进程操作, 使用到的技术有共享内存, 信号量.
//使用信号量对自加过程进行加锁, 结果不会出现错误.
#define NUM 10000000 // 1 million

int main(int argc, char const *argv[]) {
    time_t startTime = time(NULL);
    int ret = 0;
    int shmid = shmget(1000, sizeof(int), 0600 | IPC_CREAT);
    ERROR_CHECK(shmid, -1, "shmget");
    int *p = (int *)shmat(shmid, NULL, 0);
    bzero(p, sizeof(int));
    //信号量与操作初始化
    int semid = semget(1000, 1, 0600 | IPC_CREAT); //定义一个信号量集合, 该集合中只有一个信号量
    ERROR_CHECK(semid, -1, "semget");
    ret = semctl(semid, 0, SETVAL, 1); //该信号量的资源数为 1 (仅可被一个进程同时访问)
    ERROR_CHECK(ret, -1, "semctl");
    struct sembuf sopp, sopv; //定义两个信号量操作, p 操作与 v 操作
    // p 操作初始化
    bzero(&sopp, sizeof(struct sembuf));
    sopp.sem_num = 0;        //要操作的是 0 号信号量
    sopp.sem_op = -1;        //操作为资源数-1
    sopp.sem_flg = SEM_UNDO; //如果进程崩溃, 将通过该信号量进行的所有信号量进行撤销
    // v 操作初始化
    bzero(&sopv, sizeof(struct sembuf));
    sopv.sem_num = 0;        //要操作的是 0 号信号量
    sopv.sem_op = 1;         //操作为资源数+1
    sopv.sem_flg = SEM_UNDO; //如果进程崩溃, 将通过该信号量进行的所有信号量进行撤销
    // fork进程并进行自加操作
    if (fork()) { //父进程
        for (int i = 0; i < NUM; i++) {
            ret = semop(semid, &sopp, 1); //进行 p 操作
            ERROR_CHECK(ret, -1, "semop");
            (*p)++;
            ret = semop(semid, &sopv, 1); //进行 v 操作
            ERROR_CHECK(ret, -1, "semop");
        }
        wait(NULL);
        printf("result = %d\n", *p);
        ret = shmctl(shmid, IPC_RMID, NULL); //销毁共享内存
        ERROR_CHECK(ret, -1, "shmctl");
        ret = semctl(semid, IPC_RMID, 0, NULL); //销毁信号量集合
        ERROR_CHECK(ret, -1, "semctl");
        time_t endTime = time(NULL);
        printf("总用时 %ld 秒.\n", endTime - startTime);
    } else {
        for (int i = 0; i < NUM; i++) {
            ret = semop(semid, &sopp, 1); //进行 p 操作
            ERROR_CHECK(ret, -1, "semop");
            (*p)++;
            ret = semop(semid, &sopv, 1); //进行 v 操作
            ERROR_CHECK(ret, -1, "semop");
        }
    }
    return 0;
}