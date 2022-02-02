#include <func.h>
//写模式的 popen 会以写模式打开一个管道并返回该管道的文件指针, 随后拉起一个进程并将该进程的标准输入重定向至管道的读端.
int main(int argc, char const *argv[]) {
    int ret = 0;
    FILE *fp = popen("./popen_w_divide", "w");
    char buf[] = "10 5";
    fwrite(buf, 1, strlen(buf), fp);
    pclose(fp);
    return 0;
}