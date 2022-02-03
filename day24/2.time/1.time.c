#include <func.h>

int main(int argc, char const *argv[]) {
    int ret = 0;
    // 使用 time 函数获取当前距离 1970-01-01 00:00:00 +0000 (UTC) 的时间
    time_t now = time(NULL);
    printf("time = %ld\n", now);
    // 使用 ctime 函数获取当前时间
    printf("ctime = %s", ctime(&now));
    // 使用 gmtime 函数获取当前时间
    struct tm *p = gmtime(&now);
    printf("%04d-%02d-%02d %02d:%02d:%02d, the %dth day of the year, week %d\n", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour + 8, p->tm_min, p->tm_sec, p->tm_yday, p->tm_wday);
    // 使用 gettimeofday 函数获取当前时间
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("sec = %ld, usec = %ld\n", tv.tv_sec, tv.tv_usec);
    return 0;
}