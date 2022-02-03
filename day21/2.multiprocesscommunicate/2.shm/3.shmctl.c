#include <func.h>
//共享内存属性的获取与修改
int main(int argc, char const *argv[]) {
    int ret = 0;
    int shmid = shmget(1000, 1024, 0600 | IPC_CREAT); //创建共享内存
    printf("创建共享内存段成功!\n");
    struct shmid_ds buf;
    bzero(&buf, sizeof(buf));
    ret = shmctl(shmid, IPC_STAT, &buf); //获取属性
    ERROR_CHECK(ret, -1, "shmctl");
    printf("创建者 uid 为 %u, 读写权限为 %o, 共享内存大小为 %lu, 当前连接数为 %lu.\n", buf.shm_perm.cuid, buf.shm_perm.mode, buf.shm_segsz, buf.shm_nattch);
    //尝试写入共享内存, 成功
    char *p = (char *)shmat(shmid, NULL, 0);
    ERROR_CHECK(p, (void *)(-1), "shmat");
    strcpy(p, "hello");
    printf("now, there is %s\n", p);
    shmdt(p);
    //修改共享内存属性(改为只读)
    buf.shm_perm.mode = 0400;           //修改 buf 内属性为只读
    ret = shmctl(shmid, IPC_SET, &buf); //将 buf 填入共享内存属性段
    ERROR_CHECK(ret, -1, "shmctl");
    ret = shmctl(shmid, IPC_STAT, &buf); //再次获取属性
    ERROR_CHECK(ret, -1, "shmctl");
    printf("创建者 uid 为 %u, 读写权限为 %o, 共享内存大小为 %lu, 当前连接数为 %lu.\n", buf.shm_perm.cuid, buf.shm_perm.mode, buf.shm_segsz, buf.shm_nattch);
    int pid = 0;
    if ((pid = fork())) {
        int status = 0;
        waitpid(pid, &status, 0);
        printf("成功回收子进程.\n");
        ret = shmctl(shmid, IPC_RMID, NULL);
        ERROR_CHECK(ret, -1, "shmctl");
        printf("已删除共享内存段.\n");
    } else {
        printf("子进程开始:\n");
        //再次尝试写入共享内存, 失败
        char *q = (char *)shmat(shmid, NULL, 0); // shmat 在 shmflg 为 0 时要求具有读写权限, 否则返回(void*)(-1).
        ERROR_CHECK(q, (void *)(-1), "shmat");   //检测到错误并返回 -1
        //以下三行不会被执行
        strcpy(q, "hello");
        printf("now, there is %s\n", q);
        shmdt(q);
    }
    return 0;
}