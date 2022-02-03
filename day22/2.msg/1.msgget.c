#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    int msgid = msgget(1000, 0600 | IPC_CREAT);
    ERROR_CHECK(msgid, -1, "msgget");
    sleep(10);
    ret = msgctl(msgid, IPC_RMID, NULL);
    ERROR_CHECK(ret, -1, "msgctl");
    return 0;
}