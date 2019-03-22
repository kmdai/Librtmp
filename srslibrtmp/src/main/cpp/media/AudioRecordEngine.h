//
// Created by 哔哩哔哩 on 2019/3/12.
//

#ifndef LIBRTMP_AUDIORECORDENGINE_H
#define LIBRTMP_AUDIORECORDENGINE_H

#include "oboe/Oboe.h"
#include "push_utils.h"

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

    void startStream(oboe::AudioStream *stream);

    void stopStream(oboe::AudioStream *stream);

    void closeStream(oboe::AudioStream *stream);

private:
    int32_t mSampleRate = oboe::kUnspecified;
    int32_t mRecordingDeviceId = 44100;
    oboe::AudioStream *mRecordStream = nullptr;
    oboe::AudioFormat mFormat = oboe::AudioFormat::I16;
    int32_t mInputChannelCount = oboe::ChannelCount::Mono;
    oboe::AudioApi mAudioApi = oboe::AudioApi::OpenSLES;

    oboe::AudioStreamBuilder *setupRecordingStreamParameters(
            oboe::AudioStreamBuilder *builder);

    oboe::AudioStreamBuilder *setupCommonStreamParameters(
            oboe::AudioStreamBuilder *builder);
};

//extern "C" {
//long startAudioRecord() {
//    auto audioRecordEngine = new AudioRecordEngine();
//    audioRecordEngine->openRecordingStream();
//    return (long) (audioRecordEngine);
//}
//int cancelAudioRecord(long ptr) {
//    auto audioRecordEngine = (AudioRecordEngine *) (ptr);
//    delete audioRecordEngine;
//    return 0;
//}
//};
#endif //LIBRTMP_AUDIORECORDENGINE_H
