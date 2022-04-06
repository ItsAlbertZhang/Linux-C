#include "progress_bar.h"

// 初始化进度条
int progress_bar_init(struct progress_bar_t *progress_bar, size_t total_size, unsigned int flush_interval) {
    int ret = 0;

    bzero(progress_bar, sizeof(struct progress_bar_t));

    progress_bar->percent.total_size = total_size;
    progress_bar->percent.thousand_size = total_size / 1000;

    progress_bar->speed.flush_interval = flush_interval;
    // 起一个计时器线程
    ret = pthread_create(&progress_bar->speed.pthid, NULL, child_pthread_timmer, (void *)progress_bar);
    THREAD_ERROR_CHECK(ret, "pthread_create");

    gettimeofday(&progress_bar->speed.start_time, NULL); // 获取开始时间

    return 0;
}

// 计时器线程
void *child_pthread_timmer(void *argc) {
    struct progress_bar_t *progress_bar = (struct progress_bar_t *)argc;
    while (1) {
        // 每隔固定的时间将打印速度的 bool 变量设为 1
        sleep(progress_bar->speed.flush_interval);
        pthread_testcancel();
        progress_bar->speed.flush_bool = 1;
    }
}

// 销毁进度条
int progress_bar_destroy(struct progress_bar_t *progress_bar) {
    int ret = 0;

    struct timeval end_time;
    ret = gettimeofday(&end_time, NULL);

    progress_bar->speed.download_speed = progress_bar->percent.total_size * 1000000 / ((end_time.tv_sec - progress_bar->speed.start_time.tv_sec) * 1000000 + (end_time.tv_usec - progress_bar->speed.start_time.tv_usec));
    progress_bar->speed.flush_bool = 1;
    print_progress_bar(progress_bar);
    printf(" (平均速度)\n");

    ret = pthread_cancel(progress_bar->speed.pthid); // 销毁计时器线程
    THREAD_ERROR_CHECK(ret, "pthread_cancel");

    return 0;
}

// 进度条处理函数
int progress_bar_handle(struct progress_bar_t *progress_bar, size_t current_size) {
    int ret = 0;

    int flush_print = 0;
    while (current_size - progress_bar->percent.last_size > progress_bar->percent.thousand_size) {
        progress_bar->percent.last_size += progress_bar->percent.thousand_size;
        (progress_bar->percent.perthousand)++;
        if (progress_bar->percent.perthousand > 9) {
            progress_bar->percent.perthousand -= 10;
            (progress_bar->percent.percent)++;
        }
        flush_print = 1;
    }
    if (progress_bar->speed.flush_bool) {
        progress_bar->speed.download_speed = current_size - progress_bar->speed.last_second_size;
        progress_bar->speed.last_second_size = current_size;
        flush_print = 1;
    }
    if (flush_print) {
        print_progress_bar(progress_bar);
    }

    return 0;
}

// 进度条打印
int print_progress_bar(struct progress_bar_t *progress_bar) {
    static size_t download_speed_format = 0;
    static int klevel = 0;

    if (progress_bar->speed.flush_bool) {
        download_speed_format = progress_bar->speed.download_speed;
        klevel = 0;
        while (download_speed_format >= 10000) {
            klevel++;
            download_speed_format >>= 10;
        }
    }

    int x = progress_bar->percent.percent >> 2;
    printf("\r[");
    for (int i = 0; i < x; i++) {
        printf("=");
    }
    for(int i = x; i < 25; i++) {
        printf(" ");
    }
    printf("]   %3d.%1d%%     %4ld ", progress_bar->percent.percent, progress_bar->percent.perthousand, download_speed_format);
    if (klevel == 0) {
        printf(" B/s");
    } else if (klevel == 1) {
        printf("KB/s");
    } else if (klevel == 2) {
        printf("MB/s");
    } else if (klevel == 2) {
        printf("GB/s");
    } else if (klevel == 2) {
        printf("TB/s");
    }
    fflush(stdout);

    progress_bar->speed.flush_bool = 0;

    return 0;
}