#include <func.h>
//文件属于进程资源, 任意线程关闭文件后, 整个进程都会失去该文件的访问

//子线程中关闭文件
void *pthreadfunc(void *arg) { //注册一个子线程函数
    int fd = (int)(long)arg;
    close(fd);
    printf("子线程已关闭描述符编号为 %d 的文件.\n", fd);
    pthread_exit(NULL);
}

int main(int argc, const char *argv[]) {
    int ret = 0;
    int fd = open("file", O_RDONLY);//打开文件
    ERROR_CHECK(fd, -1, "open");
    printf("主线程打开文件成功, 文件描述符编号为 %d.\n", fd);
    pthread_t pthid;
    ret = pthread_create(&pthid, NULL, pthreadfunc, (void *)(long)fd);//创建子线程
    THREAD_ERR_CHECK(ret, "pthread_create");
    usleep(1000);
    char buf[1024] = {0};
    ret = read(fd, buf, sizeof(buf));//尝试读取文件
    printf("读操作返回值 %d, buf = #%s#\n", ret, buf);
    return 0;
}