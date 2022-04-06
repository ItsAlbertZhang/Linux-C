#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#include "head.h"

struct percent_t {
    size_t total_size;    // 总大小
    size_t thousand_size; // 总大小的千分之一
    size_t last_size;     // 上一次刷新进度时, 完成的大小
    size_t current_size;  // 当前执行完成的大小
    int percent;          // 百分比 (用于显示)
    int perthousand;      // 千分比 (小于 10, 用于显示)
};

struct speed_t {
    struct timeval start_time;   // 开始时间
    unsigned int flush_interval; // 速度刷新间隔 (单位: 秒)
    pthread_t pthid;             // 执行计时器任务的线程 id
    char flush_bool;             // 是否刷新下载速度
    size_t last_second_size;     // 上一次刷新速度时, 完成的大小
    size_t download_speed;       // 下载速度 (用于显示, 单位为字节/秒)
};

struct progress_bar_t {
    struct percent_t percent;
    struct speed_t speed;
};

// 初始化进度条
int progress_bar_init(struct progress_bar_t *progress_bar, size_t total_size, unsigned int flush_interval);

// 计时器线程
void *child_pthread_timmer(void *argc);

// 销毁进度条
int progress_bar_destroy(struct progress_bar_t *progress_bar);

// 进度条处理函数
int progress_bar_handle(struct progress_bar_t *progress_bar, size_t current_size);

// 进度条打印
int print_progress_bar(struct progress_bar_t *progress_bar);

#endif