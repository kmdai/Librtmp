//
// Created by kmdai on 2019/3/7.
//

#ifndef LIBRTMP_MEDIAENCODER_H
#define LIBRTMP_MEDIAENCODER_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <cstring>
#include <functional>
#include "android/log.h"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"MediaEncoder--",__VA_ARGS__);
class MediaEncoder {
public:
    MediaEncoder();

    bool init(AMediaFormat *aMediaFormat, const char *name);

    void start();

    void stop();

    void processData(uint8_t *data, uint32_t size, uint64_t presentationTimeUs);

    ~MediaEncoder();

    std::function<void(uint8_t *, uint32_t, uint64_t,int)> callback{nullptr};
private:
    AMediaCodec *aMediaCodec{nullptr};


};

#endif //LIBRTMP_MEDIAENCODER_H
