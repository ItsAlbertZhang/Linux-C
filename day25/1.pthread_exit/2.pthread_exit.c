#include <func.h>
//在子线程函数调用的函数中使用 pthread_exit 返回

void my_pthread_exit(void) {
    pthread_exit(NULL);
}

void *pthreadfunc(void *arg) { //注册一个子线程函数
    printf("here 1.\n");
    my_pthread_exit();   //调用的函数中子线程退出
    printf("here 2.\n"); //此行不会被打印
    return NULL;
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    pthread_t pthid;
    pthread_create(&pthid, NULL, pthreadfunc, NULL); //创建一个子线程
    usleep(1000);
    return 0;
}