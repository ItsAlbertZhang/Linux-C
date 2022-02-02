#include <func.h>
//读模式的 popen 会以读模式打开一个管道并返回该管道的文件指针, 随后拉起一个进程并将该进程的标准输出重定向至管道的写端.
int main(int argc, char const *argv[]) {
    int ret = 0;
    ARGS_CHECK(argc, 2);
    FILE *fp = popen(argv[1], "r");
    char buf[1024] = {0};
    fread(buf, 1, 1024, fp);
    printf("%s\n", buf);
    pclose(fp);
    return 0;
}