#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    int semid = semget(1000, 3, 0600 | IPC_CREAT); //创建信号量集合
    printf("信号量创建完毕! 睡眠10秒...\n");
    sleep(10);
    printf("继续执行...\n");
    ERROR_CHECK(ret, -1, "semget");
    //先进行 IPC_STAT 和 IPC_SET 操作
    struct semid_ds semid_ds_buf;
    bzero(&semid_ds_buf, sizeof(semid_ds_buf));
    ret = semctl(semid, 0, IPC_STAT, &semid_ds_buf); //获取信号量集合属性
    ERROR_CHECK(ret, -1, "semctl");
    printf("创建者uid为 %u, 读写权限为 %o, 信号量集合中的成员个数为 %lu.\n", semid_ds_buf.sem_perm.cuid, semid_ds_buf.sem_perm.mode, semid_ds_buf.sem_nsems);
    semid_ds_buf.sem_perm.mode = 0666;              //设定要传入的属性
    ret = semctl(semid, 0, IPC_SET, &semid_ds_buf); //设定信号量集合属性
    ERROR_CHECK(ret, -1, "semctl");
    printf("信号量属性设置完毕! 睡眠10秒...\n");
    sleep(10);
    printf("继续执行...\n");
    //进行 GETALL 和 SETALL 操作
    unsigned short arr[3] = {1, 2, 3};   //定义一个数组, 用于 SETALL
    ret = semctl(semid, 0, SETALL, arr); //执行 SETALL 操作
    ERROR_CHECK(ret, -1, "semctl");
    bzero(arr, sizeof(arr));             //清空数组, 用于 GETALL
    ret = semctl(semid, 0, GETALL, arr); //执行 GETALL 操作
    ERROR_CHECK(ret, -1, "semctl");
    for (int i = 0; i < 3; i++)
        printf("%d\t", arr[i]);
    printf("\n"); //打印输出数组
    //进行 GETVAL 和 SETVAL 操作
    ret = semctl(semid, 2, SETVAL, 100); //将第三个信号量的值设为 100
    ERROR_CHECK(ret, -1, "semctl");
    ret = semctl(semid, 2, GETVAL); //获取第三个信号量的值
    ERROR_CHECK(ret, -1, "semctl");
    printf("第 3 个信号量的值为 %d\n", ret);
    ret = semctl(semid, 0, IPC_RMID);
    return 0;
}