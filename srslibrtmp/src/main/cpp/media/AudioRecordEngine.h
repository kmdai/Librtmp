//
// Created by 哔哩哔哩 on 2019/3/12.
//

#ifndef LIBRTMP_AUDIORECORDENGINE_H
#define LIBRTMP_AUDIORECORDENGINE_H

#include "oboe/Oboe.h"

class AudioRecordEngine : public oboe::AudioStreamCallback {
public:
    ~AudioRecordEngine();
    /*
     * oboe::AudioStreamCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);

    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

private:
    int32_t mSampleRate = oboe::kUnspecified;
    int32_t mOutputChannelCount = oboe::ChannelCount::Stereo;
    oboe::AudioStream *mRecordStream = nullptr;
    void startStream(oboe::AudioStream *stream);
    void stopStream(oboe::AudioStream *stream);
    void closeStream(oboe::AudioStream *stream);
};


#endif //LIBRTMP_AUDIORECORDENGINE_H
