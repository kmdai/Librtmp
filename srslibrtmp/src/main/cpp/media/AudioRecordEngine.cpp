//
// Created by 哔哩哔哩 on 2019/3/12.
//

#include "AudioRecordEngine.h"

AudioRecordEngine::~AudioRecordEngine() {
    stopStream(mRecordStream);
    closeStream(mRecordStream);
}

/**
 *
 * @param oboeStream
 * @param audioData
 * @param numFrames
 * @return
 */
oboe::DataCallbackResult AudioRecordEngine::onAudioReady(oboe::AudioStream *oboeStream,
                                                         void *audioData, int32_t numFrames) {

    auto *data = static_cast<uint8_t *>(audioData);
    SRS_LOGE("---%04x---%04x", data[0], data[1]);
    return oboe::DataCallbackResult::Continue;
}

void AudioRecordEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::startStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->requestStart();
        if (result != oboe::Result::OK) {
            SRS_LOGE("Error starting stream. %s", oboe::convertToText(result));
        }
    }
}

void AudioRecordEngine::stopStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->stop(0L);
        if (result != oboe::Result::OK) {
            SRS_LOGE("Error stopping stream. %s", oboe::convertToText(result));
        }
    }
}

void AudioRecordEngine::closeStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->stop(0L);
        if (result != oboe::Result::OK) {
            SRS_LOGE("Error stopping stream. %s", oboe::convertToText(result));
        }
    }
}

oboe::AudioStreamBuilder *
AudioRecordEngine::setupRecordingStreamParameters(oboe::AudioStreamBuilder *builder) {
    builder->setCallback(this)
            ->setAudioApi(mAudioApi)
            ->setChannelCount(mInputChannelCount)
            ->setSampleRate(mSampleRate)
            ->setDeviceId(mRecordingDeviceId);
    return setupCommonStreamParameters(builder);
}

void AudioRecordEngine::openRecordingStream() {
    oboe::AudioStreamBuilder builder;
    setupRecordingStreamParameters(&builder);
    auto result = builder.openStream(&mRecordStream);
    if (result == oboe::Result::OK && mRecordStream) {
        startStream(mRecordStream);
    }
}

oboe::AudioStreamBuilder *
AudioRecordEngine::setupCommonStreamParameters(oboe::AudioStreamBuilder *builder) {
    // We request EXCLUSIVE mode since this will give us the lowest possible
    // latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED
    // mode.
    builder->setAudioApi(mAudioApi)
            ->setFormat(mFormat)
            ->setSharingMode(oboe::SharingMode::Exclusive)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency);
    return builder;
}

AudioRecordEngine::AudioRecordEngine() {

}
