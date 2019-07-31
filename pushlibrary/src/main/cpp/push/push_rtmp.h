//
// Created by kmdai on 18-4-19.
//

#ifndef LIBRTMP_PUSH_RTMP_H
#define LIBRTMP_PUSH_RTMP_H

#include "rtmp.h"
#include "amf.h"
#include "push_queue.h"
#include <jni.h>
#include <pthread.h>
#include "push_utils.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct mediaConfig {
    uint32_t frame_rate;
    uint32_t video_bit_rate;
    uint32_t width;
    uint32_t height;
    uint32_t audio_bit_rate;
    uint32_t audio_sample_rate;
    int32_t channel_count;
} media_config;

int init_srs(const char *url);

void add_frame(char *data, int32_t size, int32_t type, uint32_t time);

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts);

void *push_data(void *gvm);

void rtmp_start(JavaVM *gVm);

void rtmp_destroy();

void set_framerate(uint32_t framerate);

void set_VideoBitrate(uint32_t videodatarate);

void set_Width(uint32_t width);

void set_Height(uint32_t height);

void set_config(media_config *config);

void set_AudioBitrate(uint32_t audiodatarate);

void set_Channel(int32_t channel);

void set_Samplerate(uint32_t audiosamplerate);


#ifdef __cplusplus
}
#endif
#endif //LIBRTMP_PUSH_RTMP_H
