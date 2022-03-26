#ifndef func_h
#define func_h

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/sendfile.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ARGS_CHECK(argc, correct_argc) \
    {                                  \
        if (argc != correct_argc) {    \
            printf("error args.\n");   \
            return -1;                 \
        }                              \
    }
#define ERROR_CHECK(ret, error_ret, func_name)                \
    {                                                         \
        if (ret == error_ret) {                               \
            printf("%s failed, ret = %d.\n", func_name, ret); \
            perror(func_name);                                \
            return -1;                                        \
        }                                                     \
    }
#define THREAD_ERROR_CHECK(ret, func_name)                                      \
    {                                                                           \
        if (ret != 0) {                                                         \
            printf("%s failed, ret = %d, %s\n", func_name, ret, strerror(ret)); \
            return -1;                                                          \
        }                                                                       \
    }
#define CORRECT_CHECK(ret, func_name)                         \
    {                                                         \
        if (ret != 0) {                                       \
            printf("%s failed, ret = %d.\n", func_name, ret); \
            perror(func_name);                                \
            return -1;                                        \
        }                                                     \
    }

#endif
