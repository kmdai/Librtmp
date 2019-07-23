//
// Created by kmdai on 2019/3/7.
//

#ifndef LIBRTMP_MEDIAENCODER_H
#define LIBRTMP_MEDIAENCODER_H

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormat.h>
#include <cstring>

typedef struct MediaConfig {

    int width;
    int height;
    int framerate;
    int bitrate;
    int i_frame_interval;
    int color_format;
    ANativeWindow *nativeWindow;
    char *name;
} MediaConfig;

class MediaEncoder {
public:
    MediaEncoder();

    bool init(MediaConfig *);

    void start();

    void stop();

    ~MediaEncoder();

private:
    AMediaCodec *aMediaCodec;


};

#endif //LIBRTMP_MEDIAENCODER_H
