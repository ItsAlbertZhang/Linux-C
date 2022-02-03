#include <func.h>
//生产者消费者模型, 使用计数信号量

int main(int argc, char const *argv[]) {
    int ret = 0;
    //信号量与操作初始化
    int semid = semget(1000, 2, 0600 | IPC_CREAT); //定义一个信号量集合, 该集合中有两个信号量, 分别是库存数和空位数
    ERROR_CHECK(semid, -1, "semget");
    unsigned short arr[] = {0, 10};
    ret = semctl(semid, 0, SETALL, arr); //初始化库存和空位
    ERROR_CHECK(ret, -1, "semctl");
    struct sembuf sopv[2] = {{1, -1, 0}, {0, 1, 0}};
    struct sembuf sopp[2] = {{0, -1, 0}, {1, 1, 0}};
    if (fork()) {       //父进程
        usleep(500000); //睡觉0.5秒
        while (1) {
            ret = semop(semid, sopv, 2);
            ERROR_CHECK(ret, -1, "semop");
            printf("生产者: 生产完毕, 当前库存为 %d, 空位为 %d.\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            usleep(1100000); //睡觉1.1秒
        }
        //下面的这些部分不会被执行, 但是予以保留. 使用信号重定向即可解决这一问题.
        wait(NULL);
        ret = semctl(semid, IPC_RMID, 0, NULL); //销毁信号量集合
        ERROR_CHECK(ret, -1, "semctl");
    } else {
        usleep(1000000); //睡觉1秒
        while (1) {
            ret = semop(semid, sopp, 2);
            ERROR_CHECK(ret, -1, "semop");
            printf("消费者: 消费完毕, 当前库存为 %d, 空位为 %d.\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            usleep(1700000); //睡觉1.7秒
        }
    }
    return 0;
}