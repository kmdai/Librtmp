//
// Created by 哔哩哔哩 on 2019/3/12.
//

#include "AudioRecordEngine.h"

AudioRecordEngine::~AudioRecordEngine() {

}

oboe::DataCallbackResult AudioRecordEngine::onAudioReady(oboe::AudioStream *oboeStream,
                                                         void *audioData, int32_t numFrames) {
    return oboe::DataCallbackResult::Continue;
}

void AudioRecordEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::startStream(oboe::AudioStream *stream) {

}

void AudioRecordEngine::stopStream(oboe::AudioStream *stream) {

}

void AudioRecordEngine::closeStream(oboe::AudioStream *stream) {

}
