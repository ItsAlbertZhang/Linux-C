#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    int semid = semget(1000, 1, 0600 | IPC_CREAT);
    ERROR_CHECK(semid, -1, "semget");
    sleep(15);
    ret = semctl(semid, 0, IPC_RMID);
    return 0;
}