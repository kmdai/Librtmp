//
// Created by kmdai on 18-4-19.
//

#include "push_rtmp.h"
#include "AudioRecordEngine.h"
#include "push_rtmp.h"
#include <functional>
#include <string>

extern "C" {
AudioRecordEnginePtr audioRecordEnginePtr;
#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))
#define JNI_CLS_MANAGER "com/kmdai/rtmppush/LibrtmpManager"
static JavaVM *javaVM;
media_config media_config_{0};
long frames{0};
static jboolean native_setUrl(JNIEnv *env, jobject instance, jstring url) {
    const char *rtmp_url = env->GetStringUTFChars(url, JNI_FALSE);
    int result = init_srs(rtmp_url);
    if (!result) {
        return JNI_FALSE;
    }
    set_framerate(media_config_.frame_rate);
    set_VideoBitrate(media_config_.video_bit_rate);
    set_Width(media_config_.width);
    set_Height(media_config_.height);
    set_AudioBitrate(media_config_.audio_bit_rate);
    set_Channel(media_config_.channel_count);
    set_Samplerate(media_config_.audio_sample_rate);
    audioRecordEnginePtr->openRecordingStream();
    rtmp_start(javaVM);
    env->ReleaseStringUTFChars(url, rtmp_url);
    return JNI_TRUE;
}

static void
native_addFrame(JNIEnv *env, jobject instance, jbyteArray data, jint size, jint type, jint flag,
                jint time) {
    jbyte *chunk = (jbyte *) malloc(size);
    env->GetByteArrayRegion(data, 0, size, chunk);
    auto n_type = NODE_TYPE_VIDEO;
    if (type == 1) {
        n_type = NODE_TYPE_AUDIO;
    }
    q_node_p node = create_node((char *) chunk, size, n_type, flag, time);
    in_queue(node);
    free(chunk);
}
static void native_init(JNIEnv *env, jobject instance, jstring name) {
    const char *name_ = env->GetStringUTFChars(name, JNI_FALSE);
    std::string name_s{name_};
    audioRecordEnginePtr = createAudioRecordEnginePtr();
    audioRecordEnginePtr->initCodec(media_config_.audio_sample_rate,
                                    media_config_.channel_count,
                                    media_config_.audio_bit_rate,
                                    name_s,
                                    [](uint8_t *data, uint32_t size, uint64_t time, int flag) {
                                        q_node_p node = create_node((char *) data, size,
                                                                    NODE_TYPE_AUDIO, flag, time);
                                        LOGI("flag----:%d,time:%ld,size:%d", flag, time, size);
                                        in_queue(node);
                                    }
    );
    env->ReleaseStringUTFChars(name, name_);
}
static void native_release(JNIEnv *env, jobject instance) {
    rtmp_destroy();
    audioRecordEnginePtr->closeRecording();
}

static void native_setFrameRate(JNIEnv *env, jobject instance, jint framerate) {
    media_config_.frame_rate = static_cast<uint32_t >(framerate);
}

static void native_setVideoBitRate(JNIEnv *env, jobject instance, jint videodatarate) {
    media_config_.video_bit_rate = static_cast<uint32_t >(videodatarate);
}

static void native_setWidth(JNIEnv *env, jobject instance, jint width) {
    media_config_.width = static_cast<uint32_t >(width);
}

static void native_setHeight(JNIEnv *env, jobject instance, jint height) {
    media_config_.height = static_cast<uint32_t >(height);
}

static void native_setAudioBitrate(JNIEnv *env, jobject instance, jint audioBitrate) {
    media_config_.audio_bit_rate = static_cast<uint32_t >(audioBitrate);
}

static void native_setChannelCount(JNIEnv *env, jobject instance, jint channel) {
    media_config_.channel_count = static_cast<uint32_t >(channel);
}

static void native_setAudioSampleRate(JNIEnv *env, jobject instance, jint audiosamplerate) {
    media_config_.audio_sample_rate = static_cast<uint32_t >(audiosamplerate);
}


/**
 * 本地函数
 */
const JNINativeMethod srs_methods[] = {
        {"setUrl",             "(Ljava/lang/String;)Z", (void *) native_setUrl},
        {"release",            "()V",                   (void *) native_release},
        {"addFrame",           "([BIIII)V",             (void *) native_addFrame},
        {"setFrameRate",       "(I)V",                  (void *) native_setFrameRate},
        {"setVideoBitRate",    "(I)V",                  (void *) native_setVideoBitRate},
        {"setChannelCount",    "(I)V",                  (void *) native_setChannelCount},
        {"setWidth",           "(I)V",                  (void *) native_setWidth},
        {"setHeight",          "(I)V",                  (void *) native_setHeight},
        {"setAudioBitrate",    "(I)V",                  (void *) native_setAudioBitrate},
        {"setAudioSampleRate", "(I)V",                  (void *) native_setAudioSampleRate},
        {"init",               "(Ljava/lang/String;)V", (void *) native_init}
};

/**
 * 动态注册本地函数
 * @param vm
 * @param reserved
 * @return
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    JNIEnv *jenv;
    if (vm->GetEnv((void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        SRS_LOGE("Env not got");
        return JNI_ERR;
    }

    jclass clz = jenv->FindClass(JNI_CLS_MANAGER);
    if (clz == NULL) {
        SRS_LOGE("JNI_OnLoad:Class %s not found", JNI_CLS_MANAGER);
        return JNI_ERR;
    }

    if (jenv->RegisterNatives(clz, srs_methods, SRS_ARRAY_ELEMS(srs_methods))) {
        SRS_LOGE("methods not registered");
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
/**
 * 取消注册
 * @param vm
 * @param reserved
 */
JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv *jenv;
    if (vm->GetEnv((void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        SRS_LOGE("Env not got");
        return;
    }
    jclass clz = jenv->FindClass(JNI_CLS_MANAGER);
    if (clz == NULL) {
        SRS_LOGE("JNI_OnUnload:Class %s not found", JNI_CLS_MANAGER);
        return;
    }
    jenv->UnregisterNatives(clz);
}
}