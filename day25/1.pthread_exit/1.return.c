#include <func.h>
//在子线程函数中使用 return 返回

void *pthreadfunc(void *arg) { //注册一个子线程函数
    printf("here 1.\n");
    return NULL;
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    pthread_create(&pthid, NULL, pthreadfunc, NULL); //创建一个子线程
    usleep(1000);
    return 0;
}