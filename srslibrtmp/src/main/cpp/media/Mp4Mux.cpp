//
// Created by kmdai on 2019/3/29.
//

#include "Mp4Mux.h"
#include "android/log.h"

#define  LOG_TAG    "Mp4Mux.cpp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);
#define TYPE_H264_SPS 0x07
#define TYPE_H264_PPS 0x08
#define TYPE_H264_I_FRAME 0x05
#define TYPE_H264_P_FRAME 0x01
static uint32_t prefix = 0;
static bool is_set_SPS;
static bool is_set_PPS;
static uint32_t last_time = 0;

void find_prefix(uint8_t *, uint32_t);

Mp4Mux::Mp4Mux() {
    is_set_SPS = false;
    is_set_PPS = false;
}

Mp4Mux::~Mp4Mux() {

}


MP4FileHandle
Mp4Mux::initMp4File(const char *pFileName, uint32_t timeScale, uint32_t width, uint32_t height,
                    uint32_t framerate, uint32_t samplerate) {
    MP4FileHandle fileHandle = MP4Create(pFileName);
    mTimeScale = timeScale;
    mWidth = width;
    mHeight = height;
    mFramerate = framerate;
    mSimpleRate = samplerate;
    MP4SetTimeScale(fileHandle, mTimeScale);
    if (fileHandle != MP4_INVALID_FILE_HANDLE) {
        return fileHandle;
    }
    return nullptr;
}

bool Mp4Mux::writeH264data(MP4FileHandle mp4File, uint8_t *data, uint32_t len, uint32_t time) {
//    LOGI("nalu.type: %d", data[4] & 0x1f);
    if (mp4File == MP4_INVALID_FILE_HANDLE) {
        LOGI("mp4File==MP4_INVALID_FILE_HANDLE");
        return false;
    }
    if (prefix == 0) {
        find_prefix(data, len);
    }
    uint8_t type = data[prefix] & 0x01f;
    switch (type) {
        case TYPE_H264_I_FRAME:
        case TYPE_H264_P_FRAME: {
            if (mAudioTrackId == MP4_INVALID_TRACK_ID) {
                return false;
            }

            int dsize = len - 4;
            data[0] = dsize >> 24;
            data[1] = dsize >> 16;
            data[2] = dsize >> 8;
            data[3] = dsize & 0xff;
            uint32_t duration = (time - last_time) / 1000.0f * mTimeScale;
            LOGI("duration=: %d", duration);
            if (!MP4WriteSample(mp4File, mVideoTrackId, data, len, duration, 0, true)) {
                return false;
            }
            last_time = time;
            break;
        }
        case TYPE_H264_PPS: {
            uint8_t *pps = data + prefix;
            if (mVideoTrackId == MP4_INVALID_TRACK_ID || !is_set_SPS) return false;
            MP4AddH264PictureParameterSet(mp4File, mVideoTrackId, pps, len - prefix);
            is_set_PPS = true;
            break;
        }
        case TYPE_H264_SPS: {
            uint8_t *sps = data + prefix;
            mVideoTrackId = MP4AddH264VideoTrack(mp4File, mTimeScale, mTimeScale / mFramerate,
                                                 mWidth,
                                                 mHeight,
                                                 sps[1], // sps[1] AVCProfileIndication
                                                 sps[2], // sps[2] profile_compat
                                                 sps[3], // sps[3] AVCLevelIndication
                                                 3);
            if (mVideoTrackId == MP4_INVALID_TRACK_ID) return false;
            MP4SetVideoProfileLevel(mp4File, 0x07);
            MP4AddH264SequenceParameterSet(mp4File, mVideoTrackId, sps, len - prefix);
            is_set_SPS = true;
            break;
        }
        default:
            return false;
    }

    return true;
}

void find_prefix(uint8_t *data, uint32_t size) {
    int i = 0;
    while (i < size) {
        if (data[i++] == 0x00 && data[i++] == 0x00) {
            if (data[i++] == 0x01) {
                prefix = 3;
            } else {
                //计数回退
                i--;
                if (data[i++] == 0x00 && data[i++] == 0x01) {
                    prefix = 4;
                }
            }
        }
    }
}

bool Mp4Mux::cole(MP4FileHandle mp4File) {
    MP4Close(mp4File);
    return false;
}

void Mp4Mux::addSPSPPS(MP4FileHandle hMp4File, uint8_t *sps, uint32_t sps_len, uint8_t *pps,
                       uint32_t pps_len) {
    if (hMp4File == MP4_INVALID_FILE_HANDLE) {
        return;
    }
    mVideoTrackId = MP4AddH264VideoTrack(hMp4File, mTimeScale, mTimeScale / mFramerate,
                                         mWidth,
                                         mHeight,
                                         sps[1], // sps[1] AVCProfileIndication
                                         sps[2], // sps[2] profile_compat
                                         sps[3], // sps[3] AVCLevelIndication
                                         3);
    MP4SetVideoProfileLevel(hMp4File, 0x08); //  Simple Profile @ Level 3    1
    MP4AddH264SequenceParameterSet(hMp4File, mVideoTrackId, sps, sps_len);
    MP4AddH264PictureParameterSet(hMp4File, mVideoTrackId, pps, pps_len);
}

bool Mp4Mux::addTrackESConfiguration(MP4FileHandle hMp4File, uint8_t *config, uint32_t conf_len) {
    mAudioTrackId = MP4AddAudioTrack(hMp4File, mSimpleRate, 1024, MP4_MPEG4_AUDIO_TYPE);
    if (mAudioTrackId == MP4_INVALID_TRACK_ID)
        return false;
    MP4SetTrackESConfiguration(hMp4File, mAudioTrackId, config, conf_len);
    MP4SetAudioProfileLevel(hMp4File, 0x02);
    return true;
}

bool Mp4Mux::writeAACdata(MP4FileHandle mp4File, uint8_t *data, uint32_t len) {
    if (!MP4WriteSample(mp4File, mAudioTrackId, data, len, MP4_INVALID_DURATION, 0,
                        true)) {
        return false;
    }
    return true;
}

bool Mp4Mux::writeData(MP4FileHandle mp4File, uint8_t *data, uint32_t size) {
    return false;
}

