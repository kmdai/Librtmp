//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "srs_rtmp.h"
#include "AudioRecordEngine.h"
#include "push_flvenc.h"
#include <pthread.h>
#include "Mp4Mux.h"
#include <android/native_window_jni.h>

extern "C"
{
#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))
#define JNI_CLS_MANAGER "com/kmdai/srslibrtmp/SRSLibrtmpManager"
static JavaVM *javaVM;
//static long audio_record;
AudioRecordEnginePtr audioRecordEnginePtr;
Mp4MuxPtr mp4Mux;
void *mux_mp4(void *p);
jboolean setUrl(JNIEnv *env, jobject instance, jstring url) {
    const char *rtmp_url = env->GetStringUTFChars(url, JNI_FALSE);
    if (!init_srs(rtmp_url)) {
        env->ReleaseStringUTFChars(url, rtmp_url);
        SRS_LOGE("init_srs error---");
        return JNI_FALSE;
    }
    rtmp_start(javaVM);
    if (!audioRecordEnginePtr) {
        audioRecordEnginePtr = createAudioRecordEnginePtr("");
    }
    audioRecordEnginePtr->openRecordingStream();
    env->ReleaseStringUTFChars(url, rtmp_url);
    return JNI_TRUE;
}

void addFrame(JNIEnv *env, jobject instance, jbyteArray data, jint size, jint type, jint flag,
              jint time) {
    jbyte *chunk = env->GetByteArrayElements(data, NULL);
    q_node_p node = create_node((char *) chunk, size, (node_type) type, flag, time);

    in_queue(node);
    env->ReleaseByteArrayElements(data, chunk, 0);
}

void *mux_mp4(void *gVm) {
    JavaVM *gvm = (JavaVM *) gVm;
    JNIEnv *env = NULL;
    if (0 != gvm->AttachCurrentThread(&env, NULL)) {
        return (void *) 0;
    }
    q_node_p node_first = NULL;
    uint32_t frame_duration = 0;
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            break;
        }
        if (node_p->type == NODE_TYPE_VIDEO) {
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                uint32_t prefix = 0;
                int start = find_sps_pps_pos(node_p->data, node_p->size, 0, &prefix);
                int p_start = find_sps_pps_pos(node_p->data, node_p->size, start, &prefix);
                mp4Mux->writeH264data((uint8_t *) node_p->data, p_start - prefix,
                                      node_p->time);
                mp4Mux->writeH264data((uint8_t *) (node_p->data + p_start - prefix),
                                      node_p->size - p_start + prefix, node_p->time);
            } else {
                if (node_first == NULL) {
                    node_first = node_p;
                    continue;
                }
                frame_duration = node_p->time - node_first->time;
                mp4Mux->writeH264data((uint8_t *) node_first->data, node_first->size,
                                      frame_duration);
                free(node_first);
                node_first = node_p;
                continue;
            }
        } else if (node_p->type == NODE_TYPE_AUDIO) {
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                mp4Mux->addTrackESConfiguration((uint8_t *) node_p->data,
                                                node_p->size);
            } else {
                mp4Mux->writeAACdata((uint8_t *) node_p->data, node_p->size);
            }
        }
        free(node_p);
    }
    if (node_first != NULL) {
        free(node_first);
    }
    mp4Mux->cole();
    SRS_LOGE("delete mp4Mux---");
    gvm->DetachCurrentThread();
    return NULL;
}
void release(JNIEnv *env, jobject instance) {
    rtmp_destroy();
    if (audioRecordEnginePtr) {
        audioRecordEnginePtr->closeRecording();
    }
}

void setFrameRate(JNIEnv *env, jobject instance, jint framerate) {
    set_framerate(framerate);
}

void setVideoBitRate(JNIEnv *env, jobject instance, jint videodatarate) {
    setVideoBitrate(videodatarate);
}

void setWidth(JNIEnv *env, jobject instance, jint width) {
    set_width(width);
}

void setHeight(JNIEnv *env, jobject instance, jint height) {
    setHeight(height);
}

void setAudioBitrate(JNIEnv *env, jobject instance, jint audiodatarate) {
    setAudioBitrate(audiodatarate);
}

void setChannelCount(JNIEnv *env, jobject instance, jint channel) {
    seChannel(channel);
}


void setAudioSampleRate(JNIEnv *env, jobject instance, jint audiosamplesize) {
    setSamplerate(audiosamplesize);
}

void openAudioRecord(JNIEnv *env, jobject instance) {
//    audio_record = startAudioRecord();
//    audioRecordEngine=new AudioRecordEngine();
//    audioRecordEngine->openRecordingStream();
}
void startScreenRecord(JNIEnv *env, jobject instance) {
    mp4Mux = createMp4MuxPtr("/sdcard/DCIM/100ANDRO/test_11.mp4", 90000,
                             media_config_p->width, media_config_p->height,
                             media_config_p->framerate,
                             media_config_p->audiosamplerate);
    pthread_t pthread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&pthread, &attr, mux_mp4, javaVM);
}
void init(JNIEnv *env, jobject instance) {
    media_config_p = (media_config *) malloc(sizeof(media_config));
    media_config_p->sps = NULL;
    media_config_p->pps = NULL;
    init_queue();
}
void ndk_config(JNIEnv *env, jobject instance, int samplerate, int channel, int bitrate) {
}
/**
 * 本地函数
 */
const JNINativeMethod srs_methods[] = {
        {"setUrl",             "(Ljava/lang/String;)Z", (void *) setUrl},
        {"init",               "()V",                   (void *) init},
        {"release",            "()V",                   (void *) release},
        {"addFrame",           "([BIIII)V",             (void *) addFrame},
        {"setFrameRate",       "(I)V",                  (void *) setFrameRate},
        {"setVideoBitRate",    "(I)V",                  (void *) setVideoBitRate},
        {"setChannelCount",    "(I)V",                  (void *) setChannelCount},
        {"setAudioBitrate",    "(I)V",                  (void *) setAudioBitrate},
        {"setWidth",           "(I)V",                  (void *) setWidth},
        {"setHeight",          "(I)V",                  (void *) setHeight},
        {"setAudioSampleRate", "(I)V",                  (void *) setAudioSampleRate},
        {"openAudioRecord",    "()V",                   (void *) openAudioRecord},
        {"startScreenRecord",  "()V",                   (void *) startScreenRecord}
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

};