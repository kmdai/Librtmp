//
// Created by kmdai on 2019/3/12.
//

#ifndef LIBRTMP_AUDIORECORDENGINE_H
#define LIBRTMP_AUDIORECORDENGINE_H

#include "oboe/Oboe.h"

class AudioRecordEngine : public oboe::AudioStreamCallback {
public:
    ~AudioRecordEngine();

    AudioRecordEngine();

    /*
     * oboe::AudioStreamCallback interface implementation
     */
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames);

    void onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    void openRecordingStream();

    void closeRecording();

    void startStream(oboe::AudioStream *stream);

    void stopStream(oboe::AudioStream *stream);

    void closeStream(oboe::AudioStream *stream);

private:
    int32_t mSampleRate = 44100;
    int32_t mRecordingDeviceId = oboe::kUnspecified;
    oboe::AudioStream *mRecordStream = nullptr;
    oboe::AudioFormat mFormat = oboe::AudioFormat::I16;
    int32_t mInputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioApi mAudioApi = oboe::AudioApi::OpenSLES;

    oboe::AudioStreamBuilder *setupRecordingStreamParameters(
            oboe::AudioStreamBuilder *builder);

    oboe::AudioStreamBuilder *setupCommonStreamParameters(
            oboe::AudioStreamBuilder *builder);
};

using AudioRecordEnginePtr =std::shared_ptr<AudioRecordEngine>;


AudioRecordEnginePtr createAudioRecordEnginePtr();

#endif //LIBRTMP_AUDIORECORDENGINE_H
