//
// Created by kmdai on 2019/3/29.
//

#ifndef LIBRTMP_MP4MUX_H
#define LIBRTMP_MP4MUX_H

#include "mp4v2/mp4v2.h"
#include "string"
#include <memory>
//#include "push_utils.h"



struct Mp4Context {
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


class Mp4Mux {
public:
    Mp4Mux(const char *pFileName, uint32_t timeScal, uint32_t width, uint32_t height,
           uint32_t framerate, uint32_t samplerate);

    ~Mp4Mux();


    MP4FileHandle
    initMp4File(const char *pFileName, uint32_t timeScal, uint32_t width, uint32_t height,
                uint32_t framerate, uint32_t samplerate);

    bool writeH264data(uint8_t *data, uint32_t len, uint32_t time);

    bool writeAACdata(uint8_t *data, uint32_t len);

    void addSPSPPS(uint8_t *sps, uint32_t sps_len, uint8_t *pps,
                   uint32_t pps_len);

    bool addTrackESConfiguration(uint8_t *config, uint32_t conf_len);

    bool cole();

    bool writeData(uint8_t *data, uint32_t size);

private:
    uint32_t mTimeScale = 90000;
    MP4TrackId mVideoTrackId;
    MP4TrackId mAudioTrackId;
    MP4FileHandle mMP4FileHandle;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mFramerate;
    uint32_t mSimpleRate;
};

using Mp4MuxPtr= std::shared_ptr<Mp4Mux>;

Mp4MuxPtr createMp4MuxPtr(const char *pFileName, uint32_t timeScal, uint32_t width, uint32_t height,
                          uint32_t framerate, uint32_t samplerate) ;

#endif //LIBRTMP_MP4MUX_H
