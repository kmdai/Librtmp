//
// Created by kmdai on 18-4-19.
//

#ifndef LIBRTMP_PUSH_RTMP_H
#define LIBRTMP_PUSH_RTMP_H

#include "srs_librtmp.hpp"
#include "push_queue.h"
#include <jni.h>
#include <pthread.h>
#include "push_utils.h"


typedef struct mediaConfig {
    double framerate;
    double videodatarate;
    double width;
    double height;
    double audiodatarate;
    double audiosamplerate;
    double audiosamplesize;
    int32_t channel_count;
} media_config;

int init_srs(const char *url);

void add_frame(char *data, int32_t size, int32_t type, uint32_t time);

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts);

void *push_data(void *gvm);

void rtmp_start(JavaVM *gVm);

void rtmp_destroy();

void set_framerate(double framerate);

void set_videodatarate(double videodatarate);

void set_width(double width);

void set_height(double height);

void set_audiodatarate(double audiodatarate);

void set_audiochannel(int32_t);

void set_audiosamplerate(double audiosamplerate);

void set_audiosamplesize(double audiosamplesize);

#endif //LIBRTMP_PUSH_RTMP_H
