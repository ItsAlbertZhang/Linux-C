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
    bzero(&msg, sizeof(struct msgbuf));
    msgrcv(msgid, &msg, sizeof(struct msgbuf), 1, 0);
    printf("%s\n", msg.buf);
    return 0;
}