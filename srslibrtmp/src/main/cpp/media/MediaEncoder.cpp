//
// Created by 哔哩哔哩 on 2019/3/7.
//

#include "MediaEncoder.h"

MediaEncoder::MediaEncoder() {

}

bool MediaEncoder::init(MediaConfig *config) {
    AMediaFormat *aMediaFormat = AMediaFormat_new();
    AMediaFormat_getInt32(aMediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, &config->color_format);
    AMediaFormat_getInt32(aMediaFormat, AMEDIAFORMAT_KEY_FRAME_RATE, &config->framerate);
    AMediaFormat_getInt32(aMediaFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,
                          &config->i_frame_interval);
    AMediaFormat_getInt32(aMediaFormat, AMEDIAFORMAT_KEY_BIT_RATE, &config->bitrate);
    aMediaCodec = AMediaCodec_createCodecByName(config->name);
    AMediaCodec_configure(aMediaCodec, aMediaFormat, nullptr, nullptr,
                          AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    return true;
}

void MediaEncoder::start() {
}

void MediaEncoder::stop() {

}