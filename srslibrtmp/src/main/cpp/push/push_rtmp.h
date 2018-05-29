//
// Created by kmdai on 18-4-19.
//

#ifndef LIBRTMP_PUSH_RTMP_H
#define LIBRTMP_PUSH_RTMP_H

#include "srs_librtmp.hpp"
#include "push_queue.h"
#include <jni.h>
#include <pthread.h>
#include <android/log.h>

#define SRS_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "push", __VA_ARGS__))

typedef struct mediaConfig {
    int framerate;
    int videodatarate;
    int width;
    int height;
    int audiodatarate;
    int audiosamplerate;
    int audiosamplesize;
} media_config;

int init_srs(const char *url);

void add_frame(char *data, int32_t size, int32_t type, uint32_t time);

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts);

void *push_data(void *gvm);

void rtmp_start(JavaVM *gVm);

void rtmp_destroy();

#endif //LIBRTMP_PUSH_RTMP_H
