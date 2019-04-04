//
// Created by kmdai on 2019/3/29.
//

#ifndef LIBRTMP_MP4MUX_H
#define LIBRTMP_MP4MUX_H

#include "mp4v2/mp4v2.h"
#include "string"
#include "push_utils.h"

struct _mp4_context {
    uint32_t width;
    uint32_t height;
    uint32_t frame_rate;
    uint8_t *sps;
    uint32_t sps_len;
    uint8_t *pps;
    uint32_t pps_len;
    uint32_t bit_rate;
    uint32_t simple_rate;
    uint32_t sampleLenFieldSizeMinusOne;
};

using Mp4Context=struct _mp4_context;

class Mp4Mux {
public:
    Mp4Mux();
    ~Mp4Mux();

    void setMp4Context(Mp4Context *mp4Context);

    bool initMp4File(std::string path);

    bool writeH264data(uint8_t *data, uint32_t len);

    bool writeAACdata(uint8_t *data, uint32_t len);

private:
    Mp4Context *mMp4Context;
    MP4FileHandle mFilehandle;
    uint32_t mTimeScale = 9000;
    MP4TrackId mVideoTrackId;
    MP4TrackId mAudioTrackId;
};


#endif //LIBRTMP_MP4MUX_H
