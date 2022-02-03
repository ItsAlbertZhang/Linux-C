#include <func.h>

int dfs(const char *path, int depth) {
    DIR *dir = NULL;
    dir = opendir(path);
    ERROR_CHECK(dir, NULL, "opendir")
    struct dirent *p = NULL;
    char buf[1024] = {0};
    while(p = readdir(dir)) {
        if(strcmp(p->d_name, ".") && strcmp(p->d_name, "..")) {
            printf("%*s%s\n", 4 * depth, "", p->d_name);
            if(p->d_type == 4) {
                memset(buf, 0, 1024);
                sprintf(buf, "%s%s%s", path, "/", p->d_name);
                dfs(buf, depth + 1);
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    ARGS_CHECK(argc, 2)
    printf("%s\n", argv[1]);
    dfs(argv[1], 1);
    return 0;
}