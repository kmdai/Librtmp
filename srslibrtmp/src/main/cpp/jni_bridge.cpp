//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push/push_rtmp.h"
#include "media/AudioRecordEngine.h"
#include "push_flvenc.h"
#include <pthread.h>
#include "media/Mp4Mux.h"

extern "C"
{
#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))
#define JNI_CLS_MANAGER "com/kmdai/srslibrtmp/SRSLibrtmpManager"
static JavaVM *javaVM;
//static long audio_record;
//AudioRecordEngine* audioRecordEngine;
Mp4Mux *mp4Mux;
MP4FileHandle mp4FileHandle = MP4_INVALID_FILE_HANDLE;
void *mux(void *p);
jboolean setUrl(JNIEnv *env, jobject instance, jstring url) {
    const char *rtmp_url = env->GetStringUTFChars(url, 0);
    int result = init_srs(rtmp_url);
    if (result != 0) {
//        rtmp_start(javaVM);
        mp4Mux = new Mp4Mux();
        pthread_t pthread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_create(&pthread, &attr, mux, javaVM);
    }
    env->ReleaseStringUTFChars(url, rtmp_url);
    return result != 0 ? JNI_TRUE : JNI_FALSE;
}

void addFrame(JNIEnv *env, jobject instance, jbyteArray data, jint size, jint type, jint flag,
              jint time) {
    jbyte *chunk = env->GetByteArrayElements(data, NULL);
    q_node_p node = create_node((char *) chunk, size, (node_type) type, flag, time);

    if (node->type == NODE_TYPE_VIDEO && node->flag == NODE_FLAG_CODEC_CONFIG) {
        uint32_t prefix = 0;
        int start = find_sps_pps_pos(node->data, node->size, 0, &prefix);
        int p_start = find_sps_pps_pos(node->data, node->size, start, &prefix);
        media_config_p->sps_len = p_start - start - prefix;
        media_config_p->pps_len = node->size - p_start;
        media_config_p->sps = (uint8_t *) malloc(media_config_p->sps_len);
        media_config_p->pps = (uint8_t *) malloc(media_config_p->pps_len);
        memcpy(media_config_p->sps, node->data + start, media_config_p->sps_len);
        memcpy(media_config_p->pps, node->data + p_start, media_config_p->pps_len);
        if (mp4FileHandle == MP4_INVALID_FILE_HANDLE) {
            mp4FileHandle = mp4Mux->initMp4File("/sdcard/DCIM/100ANDRO/test_4.mp4", 90000,
                                                media_config_p->width, media_config_p->height,
                                                media_config_p->framerate, 44100);
        }

        mp4Mux->addSPSPPS(mp4FileHandle, media_config_p->sps, media_config_p->sps_len,
                          media_config_p->pps,
                          media_config_p->pps_len);
    }
    if (node->type == NODE_TYPE_AUDIO && node->flag == NODE_FLAG_CODEC_CONFIG) {
        if (mp4FileHandle == MP4_INVALID_FILE_HANDLE) {
            mp4FileHandle = mp4Mux->initMp4File("/sdcard/DCIM/100ANDRO/test_3.mp4", 90000,
                                                media_config_p->width, media_config_p->height,
                                                media_config_p->framerate, 44100);
        }
        mp4Mux->SetTrackESConfiguration(mp4FileHandle, (uint8_t *) node->data, node->size);
    }
    in_queue(node);
    env->ReleaseByteArrayElements(data, chunk, 0);
}

void *mux(void *gVm) {
    JavaVM *gvm = (JavaVM *) gVm;
    JNIEnv *env = NULL;
    if (0 != gvm->AttachCurrentThread(&env, NULL)) {
        return (void *) 0;
    }
    sleep(500);
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            break;
        }
        if (node_p->type == NODE_TYPE_VIDEO && node_p->flag != NODE_FLAG_CODEC_CONFIG) {
            mp4Mux->writeH264data(mp4FileHandle, (uint8_t *) node_p->data, node_p->size);
        } else  if (node_p->type == NODE_TYPE_AUDIO){
            mp4Mux->writeAACdata(mp4FileHandle,(uint8_t *) node_p->data, node_p->size);
        }
        free(node_p);
    }
    mp4Mux->cole(mp4FileHandle);
    delete mp4Mux;
    gvm->DetachCurrentThread();
    return NULL;
}
void release(JNIEnv *env, jobject instance) {
    rtmp_destroy();
    SRS_LOGE("delete mp4Mux---");
//    delete mp4Encoder;
//    delete audioRecordEngine;
}

void setFrameRate(JNIEnv *env, jobject instance, jdouble framerate) {
    set_framerate(framerate);
}

void setVideodatarate(JNIEnv *env, jobject instance, jdouble videodatarate) {
    set_videodatarate(videodatarate);
}

void setWidth(JNIEnv *env, jobject instance, jdouble width) {
    set_width(width);
}

void setHeight(JNIEnv *env, jobject instance, jdouble height) {
    set_height(height);
}

void setAudiodatarate(JNIEnv *env, jobject instance, jdouble audiodatarate) {
    set_audiodatarate(audiodatarate);
}

void setChannelCount(JNIEnv *env, jobject instance, jint channel) {
    set_audiochannel(channel);
}

void setAudiosamplerate(JNIEnv *env, jobject instance, jdouble audiosamplerate) {
    set_audiosamplerate(audiosamplerate);
}

void setAudiosamplesize(JNIEnv *env, jobject instance, jdouble audiosamplesize) {
    set_audiosamplesize(audiosamplesize);
}

void openAudioRecord(JNIEnv *env, jobject instance) {
//    audio_record = startAudioRecord();
//    audioRecordEngine=new AudioRecordEngine();
//    audioRecordEngine->openRecordingStream();
}

/**
 * 本地函数
 */
const JNINativeMethod srs_methods[] = {
        {"setUrl",             "(Ljava/lang/String;)Z", (void *) setUrl},
        {"release",            "()V",                   (void *) release},
        {"addFrame",           "([BIIII)V",             (void *) addFrame},
        {"setFrameRate",       "(D)V",                  (void *) setFrameRate},
        {"setVideodatarate",   "(D)V",                  (void *) setVideodatarate},
        {"setChannelCount",    "(I)V",                  (void *) setChannelCount},
        {"setWidth",           "(D)V",                  (void *) setWidth},
        {"setHeight",          "(D)V",                  (void *) setHeight},
        {"setAudiodatarate",   "(D)V",                  (void *) setAudiodatarate},
        {"setAudiosamplerate", "(D)V",                  (void *) setAudiosamplerate},
        {"setAudiosamplesize", "(D)V",                  (void *) setAudiosamplesize},
        {"openAudioRecord",    "()V",                   (void *) openAudioRecord}
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