#include <func.h>

int main(int argc, char* argv[]) {
    ARGS_CHECK(argc,2)
    struct stat buf;
    int ret = 0;
    ret = stat(argv[1], &buf);
    ERROR_CHECK(ret, -1, "stat")
    printf("ino = %ld, mode = %x, nlink = %ld, uid = %d, gid  = %d, size = %ld, time = %ld\n",\
     buf.st_ino, buf.st_mode, buf.st_nlink, buf.st_uid, buf.st_gid, buf.st_size, buf.st_mtime);
    return 0;
}