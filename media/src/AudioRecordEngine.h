//
// Created by kmdai on 2019/3/12.
//

#ifndef LIBRTMP_AUDIORECORDENGINE_H
#define LIBRTMP_AUDIORECORDENGINE_H

#include "oboe/Oboe.h"
#include "MediaEncoder.h"
#include <string>
#include <media/NdkMediaFormat.h>

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

    void initCodec(uint32_t sampleRate, uint32_t channel, uint32_t bitRate, std::string name,
                   std::function<void(uint8_t *, uint32_t, uint64_t, int)> callback);

private:
    int32_t mSampleRate = 44100;
    int32_t mChannel = 1;
    int32_t mBitRate = 96000;
    int32_t mRecordingDeviceId = oboe::kUnspecified;
    oboe::AudioStream *mRecordStream = nullptr;
    oboe::AudioFormat mFormat = oboe::AudioFormat::I16;
    int32_t mInputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioApi mAudioApi = oboe::AudioApi::OpenSLES;
    MediaEncoder mMediaEncoder;

    oboe::AudioStreamBuilder *setupRecordingStreamParameters(
            oboe::AudioStreamBuilder *builder);

    oboe::AudioStreamBuilder *setupCommonStreamParameters(
            oboe::AudioStreamBuilder *builder);

    int64_t mStartTime{0};

    int64_t systemnanotime();
};

using AudioRecordEnginePtr =std::shared_ptr<AudioRecordEngine>;


AudioRecordEnginePtr createAudioRecordEnginePtr();

#endif //LIBRTMP_AUDIORECORDENGINE_H
