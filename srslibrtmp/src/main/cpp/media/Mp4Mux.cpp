//
// Created by kmdai on 2019/3/29.
//

#include "Mp4Mux.h"
#include "android/log.h"

#define  LOG_TAG    "Mp4Mux.cpp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__);

Mp4Mux::Mp4Mux() {

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

bool Mp4Mux::writeH264data(MP4FileHandle mp4File, uint8_t *data, uint32_t len) {
    LOGI("nalu.type: %d", data[4] & 0x1f);
    int dsize = len - 4;
    data[0] = dsize >> 24;
    data[1] = dsize >> 16;
    data[2] = dsize >> 8;
    data[3] = dsize & 0xff;
    if (!MP4WriteSample(mp4File, mVideoTrackId, data, len, MP4_INVALID_DURATION, 0,
                        true)) {
        return false;
    }
    return true;
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

void Mp4Mux::SetTrackESConfiguration(MP4FileHandle hMp4File, uint8_t *config, uint32_t conf_len) {
    mAudioTrackId = MP4AddAudioTrack(hMp4File, mSimpleRate, 1024, MP4_MPEG4_AUDIO_TYPE);
    MP4SetTrackESConfiguration(hMp4File, mAudioTrackId, config, conf_len);
}

bool Mp4Mux::writeAACdata(MP4FileHandle mp4File, uint8_t *data, uint32_t len) {
    return false;
}
