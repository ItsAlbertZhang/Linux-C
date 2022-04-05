#include "lib_readconfig.h"

int get_a_new_line(FILE *f, char *s) {
    char *ret = NULL;

    ret = fgets(s, MAX_CONFIG_LENGTH, f);

    if (ret == NULL) {
        return -1;
    } else {
        s[strlen(s) - 1] = 0; // 替换行尾的 \n 为 \0
    }

    return 0;
}

char *read_config_file(char *config_file_name) {
    int ret = 0;
    FILE *config_file = fopen(config_file_name, "rb"); // 打开配置文件
    if (NULL == config_file) {
        printf("配置文件不存在!\n");
        abort();
    }
    char *conf = (char *)malloc(sizeof(char) * MAX_CONFIG_SETTINGS * MAX_CONFIG_LENGTH);

    int i = 0;
    while (i < MAX_CONFIG_SETTINGS && !ret) {
        printf("i = %d, conf[i] = %p\n", i, conf + i * MAX_CONFIG_LENGTH);
        ret = get_a_new_line(config_file, conf + i * MAX_CONFIG_LENGTH);
        i++;
    }

    fclose(config_file);
    return conf;
}