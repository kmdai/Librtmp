//
// Created by kmdai on 2019/3/29.
//

#ifndef LIBRTMP_MP4MUX_H
#define LIBRTMP_MP4MUX_H

#include "mp4v2/mp4v2.h"
#include "string"
//#include "push_utils.h"

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


    MP4FileHandle
    initMp4File(const char *pFileName, uint32_t timeScal, uint32_t width, uint32_t height,
                uint32_t framerate, uint32_t samplerate);

    bool writeH264data(MP4FileHandle mp4File, uint8_t *data, uint32_t len);
    bool writeAACdata(MP4FileHandle mp4File, uint8_t *data, uint32_t len);

    void addSPSPPS(MP4FileHandle hMp4File, uint8_t *sps, uint32_t sps_len, uint8_t *pps,
                   uint32_t pps_len);

    void SetTrackESConfiguration(MP4FileHandle hMp4File, uint8_t *config, uint32_t conf_len);

    bool cole(MP4FileHandle mp4File);

private:
    uint32_t mTimeScale = 90000;
    MP4TrackId mVideoTrackId;
    MP4TrackId mAudioTrackId;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mFramerate;
    uint32_t mSimpleRate;
};


#endif //LIBRTMP_MP4MUX_H
