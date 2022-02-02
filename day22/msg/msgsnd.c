#include <func.h>

struct msgbuf {
    long mtype;
    char buf[64];
};

int main(int argc, char const *argv[]) {
    int ret = 0;
    int msgid = msgget(1000, 0600 | IPC_CREAT);
    ERROR_CHECK(msgid, -1, "msgget");
    struct msgbuf msg;
    msg.mtype = 1;
    strcpy(msg.buf, "hello, world");
    ret = msgsnd(msgid, &msg, strlen(msg.buf), 0);
    ERROR_CHECK(ret, -1, "msgsnd");
    return 0;
}