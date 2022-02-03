#include <func.h>
//格式化输出 文件类型转换 用户id和组id转换 时间转换
int main(int argc, char const *argv[]) {
    char path[1024] = {0};
    struct dirent *p = NULL;
    if(argc == 1) {
        getcwd(path, sizeof(path));
    } else if(argc == 2) {
        sprintf(path, "%s", argv[1]);
    } else {
        return -1;
    }
    DIR *dir = opendir(path);
    struct stat buf;
    int ret = 0;
    while((p = readdir(dir))) {
        ret = stat(p->d_name, &buf);
        ERROR_CHECK(ret, -1, "stat");
        //将st_mode转为字符串
        char mode[11] = "----------";
        char moodeall[11] = "drwxrwxrwx";
        mode_t temp = buf.st_mode;
        int i = 1;
        for(int cnt = 9; cnt > 0; cnt--) {
            if(i&buf.st_mode) {
                mode[cnt] = moodeall[cnt];
            }
            i <<= 1;
        }
        i <<= 5;
        if(i&buf.st_mode) {
            mode[0] = moodeall[0];
        }
        //获取uid和gid对应的结构体
        struct group *grp = getgrgid(buf.st_gid);
        struct passwd *pwd = getpwuid(buf.st_uid);
        //获取时间
        char *time = ctime(&buf.st_mtime);
        time += 4;  time[15] = 0;
        //打印输出, 再写个sort就完美了
        printf("%7ld %s %2ld %5s %5s %7ld %s %s\n",buf.st_ino, mode, buf.st_nlink, grp->gr_name, pwd->pw_name, buf.st_size, time, p->d_name);
    }
    return 0;
}
