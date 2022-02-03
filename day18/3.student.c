#include <func.h>

typedef struct {
    int ID;
    char name[10];
    double score;
} stu;

int main(void) {
    stu arr[3] = {{1001, "zhangsan", 95.2}, {1002, "lisi", 96.1}, {1003, "wangwu", 97.0}};//定义数组
    int fd = open("stufile", O_RDWR|O_CREAT|O_TRUNC, 0775);//打开文件
    ERROR_CHECK(fd, -1, "open")
    ftruncate(fd,sizeof(stu) * 3);//设置文件大小
    stu *p = (stu*)mmap(NULL, sizeof(stu) * 3, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);//mmap映射
    memcpy(p, arr, sizeof(stu) * 3);//内存复制
    int intret = munmap(p, sizeof(stu) * 3);//关闭映射,写入文件
    lseek(fd, 0, SEEK_SET);//光标偏移
    stu buf;//设置缓冲区用于打印
    for(int i = 0; i < 3; i++) {
        read(fd, &buf, sizeof(stu));//读一个stu的大小
        printf("id = %d, name = %-10s, score = %4.2lf\n", buf.ID, buf.name, buf.score);//打印输出
    }
    return 0;
}