#ifndef getconfig_h
#define getconfig_h

#include <func.h>

struct config_t {
    char ip[16];
    char port[8];
    char child_process_number[4];
};

// 读取配置文件
int getconfig(FILE *configfile, struct config_t *pconf);

#endif