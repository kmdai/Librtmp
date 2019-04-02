//
// Created by kmdai on 2019/3/29.
//

#include "Mp4Mux.h"

void Mp4Mux::setMp4Context(Mp4Context *mp4Context) {
    this->mMp4Context = mp4Context;
}

bool Mp4Mux::initMp4File(std::string path) {
    mFilehandle = MP4Create(path.data());
    if (mFilehandle == MP4_INVALID_FILE_HANDLE)return false;
    if (!MP4SetTimeScale(mFilehandle, 90000)) return false;
    if (mMp4Context) {
        mVideoTrackId = MP4AddH264VideoTrack(mFilehandle, 90000, 90000 / mMp4Context->frame_rate,
                                             mMp4Context->width, mMp4Context->height,
                                             mMp4Context->sps[1],
                                             mMp4Context->sps[2],
                                             mMp4Context->sps[3],
                                             mMp4Context->sampleLenFieldSizeMinusOne);
        mVideoTrackId = MP4AddAudioTrack(mFilehandle, mMp4Context->simple_rate, 1024,
                                         MP4_MPEG2_AAC_AUDIO_TYPE);
        if (mVideoTrackId == MP4_INVALID_TRACK_ID) {
            SRS_LOGE("mVideoTrackId==MP4_INVALID_TRACK_ID");
            return false;
        }
        if (mAudioTrackId == MP4_INVALID_TRACK_ID) {
            SRS_LOGE("mAudioTrackId==MP4_INVALID_TRACK_ID");
            return false;
        }
        MP4AddH264SequenceParameterSet(mFilehandle, mVideoTrackId, mMp4Context->sps,
                                       mMp4Context->sps_len);
        MP4AddH264PictureParameterSet(mFilehandle, mVideoTrackId, mMp4Context->pps,
                                      mMp4Context->pps_len);
        MP4SetVideoProfileLevel(mFilehandle, 3);
        MP4SetAudioProfileLevel(mFilehandle, 0x02);
        return true;
    }
    return false;
}

bool Mp4Mux::writeH264data(uint8_t *data, uint32_t len, bool isIFrame) {
    if (mFilehandle == MP4_INVALID_FILE_HANDLE)return false;
    if (mMp4Context->sampleLenFieldSizeMinusOne > 4) {
        SRS_LOGE("mMp4Context->sampleLenFieldSizeMinusOne>4");
        mMp4Context->sampleLenFieldSizeMinusOne = 4;
    }
    uint32_t nlu_size = len - mMp4Context->sampleLenFieldSizeMinusOne;
    int i = 0;
    while (i < mMp4Context->sampleLenFieldSizeMinusOne) {
        data[i++] = nlu_size >> (8 * (mMp4Context->sampleLenFieldSizeMinusOne - i));
    }
    MP4WriteSample(mFilehandle, mVideoTrackId, data, len, MP4_INVALID_DURATION, 0, isIFrame);
}
