#ifndef __LIB_READCONFIG_H__
#define __LIB_READCONFIG_H__

#include "head.h"

#define MAX_CONFIG_SETTINGS 16
#define MAX_CONFIG_LENGTH 64

char *read_config_file(char *config_file_name);

#endif