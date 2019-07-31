//
// Created by kmdai on 2019/3/12.
//

#include "AudioRecordEngine.h"

AudioRecordEngine::~AudioRecordEngine() {
}

AudioRecordEngine::AudioRecordEngine() {

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

    auto *data = static_cast<short *>(audioData);
    frames += numFrames;
    mMediaEncoder.processData((uint8_t *) data, numFrames * mChannel * 2,
                              (frames * 1000 / mSampleRate));
    return oboe::DataCallbackResult::Continue;
}

void AudioRecordEngine::onErrorBeforeClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
}

void AudioRecordEngine::startStream(oboe::AudioStream *stream) {
    if (stream) {
        mStartTime = systemnanotime();
        oboe::Result result = stream->requestStart();
        if (result != oboe::Result::OK) {
        } else {

        }
    }
}

void AudioRecordEngine::stopStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->stop(0L);
        if (result != oboe::Result::OK) {
        } else {
        }
        mMediaEncoder.stop();
    }
}

void AudioRecordEngine::closeStream(oboe::AudioStream *stream) {
    if (stream) {
        oboe::Result result = stream->close();
        if (result != oboe::Result::OK) {
        } else {
        }
    }
}

oboe::AudioStreamBuilder *
AudioRecordEngine::setupRecordingStreamParameters(oboe::AudioStreamBuilder *builder) {
    builder->setCallback(this)
            ->setDeviceId(mRecordingDeviceId)
            ->setDirection(oboe::Direction::Input)
            ->setChannelCount(mInputChannelCount)
            ->setFramesPerCallback(1024)
            ->setSampleRate(mSampleRate);
    return setupCommonStreamParameters(builder);
}

void AudioRecordEngine::openRecordingStream() {
    oboe::AudioStreamBuilder builder;
    if (builder.isAAudioSupported()) {
        mAudioApi = oboe::AudioApi::AAudio;
    }
    setupRecordingStreamParameters(&builder);
    auto result = builder.openStream(&mRecordStream);
    if (result == oboe::Result::OK && mRecordStream) {
        mMediaEncoder.start();
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
            ->setSharingMode(oboe::SharingMode::Shared)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency);
    return builder;
}


void AudioRecordEngine::closeRecording() {
    stopStream(mRecordStream);
    closeStream(mRecordStream);
}

void AudioRecordEngine::initCodec(uint32_t sampleRate, uint32_t channel, uint32_t bitRate,
                                  std::string name,
                                  std::function<void(uint8_t *, uint32_t, uint64_t,
                                                     int)> callback) {
    auto format = AMediaFormat_new();
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_AAC_PROFILE, 2);//AACObjectLC         = 2;
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, mSampleRate);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, channel);
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "audio/mp4a-latm");
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BIT_RATE, bitRate);
    LOGI("AudioRecordEngine::initCodec,name:%s,samplerate:%d,channel:%d,bitrate:%d", name.data(),
         sampleRate, channel, bitRate);
    mMediaEncoder.init(format, name.data());
    frames = 0;
    mMediaEncoder.callback = callback;
    AMediaFormat_delete(format);
}

int64_t AudioRecordEngine::systemnanotime() {
    timespec now{0, 0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}

AudioRecordEnginePtr createAudioRecordEnginePtr() {
    return std::make_shared<AudioRecordEngine>();
}
