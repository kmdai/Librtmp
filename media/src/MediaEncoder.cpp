//
// Created by kmdai on 2019/3/7.
//

#include "MediaEncoder.h"

MediaEncoder::MediaEncoder() {

}

MediaEncoder::~MediaEncoder() {

}

bool MediaEncoder::init(AMediaFormat *aMediaFormat, const char *name) {
    aMediaCodec = AMediaCodec_createCodecByName(name);
    AMediaCodec_configure(aMediaCodec, aMediaFormat, nullptr, nullptr,
                          AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    return true;
}

void MediaEncoder::processData(uint8_t *data, uint32_t size, uint64_t presentationTimeUs) {
    ssize_t bufidx = -1;

    bufidx = AMediaCodec_dequeueInputBuffer(aMediaCodec, 2000);
    if (bufidx >= 0) {
        size_t bufsize;
        auto buf = AMediaCodec_getInputBuffer(aMediaCodec, bufidx, &bufsize);
        LOGI("---size:%d,time:%ld,bufsize:%d", size, presentationTimeUs, bufsize);
        memcpy(buf, data, size);
        AMediaCodec_queueInputBuffer(aMediaCodec, bufidx, 0, size, presentationTimeUs, 0);
    }
    AMediaCodecBufferInfo info;
    auto status = AMediaCodec_dequeueOutputBuffer(aMediaCodec, &info, 0);
    if (status >= 0) {
        if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
            return;
        }
        size_t bufsize;
        auto buf = AMediaCodec_getOutputBuffer(aMediaCodec, status, &bufsize);
        if (callback != nullptr) {
            auto dst = (uint8_t *) malloc(info.size);
            memcpy(dst, buf + info.offset, info.size);
            callback(dst, info.size, info.presentationTimeUs, info.flags);
            free(dst);
        }
        AMediaCodec_releaseOutputBuffer(aMediaCodec, status, 0);
    } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
    } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
        auto format = AMediaCodec_getOutputFormat(aMediaCodec);
        AMediaFormat_delete(format);
    } else if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
    } else {
    }
}

void MediaEncoder::start() {
    if (aMediaCodec != nullptr) {
        LOGI("MediaEncoder::start---");
        AMediaCodec_start(aMediaCodec);
    }
}

void MediaEncoder::stop() {
    if (aMediaCodec != nullptr) {
        LOGI("MediaEncoder::stop---");
        AMediaCodec_stop(aMediaCodec);
    }
}

