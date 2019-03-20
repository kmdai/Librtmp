//
// Created by kmdai on 18-4-19.
//

#include "push_rtmp.h"

#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))
#define JNI_CLS_MANAGER "com/kmdai/srslibrtmp/SRSLibrtmpManager"
static JavaVM *javaVM;

jboolean setUrl(JNIEnv *env, jobject instance, jstring url) {
    const char *rtmp_url = (*env)->GetStringUTFChars(env, url, 0);
    int result = init_srs(rtmp_url);
    if (result != 0) {
        rtmp_start(javaVM);
    }
    (*env)->ReleaseStringUTFChars(env, url, rtmp_url);
    return result != 0 ? JNI_TRUE : JNI_FALSE;
}

void addFrame(JNIEnv *env, jobject instance, jbyteArray data, jint size, jint type, jint flag,
              jint time) {
    jbyte *chunk = (*env)->GetByteArrayElements(env, data, NULL);
    q_node_p node = create_node((char *) chunk, size, type, flag, time);
    in_queue(node);
    (*env)->ReleaseByteArrayElements(env, data, chunk, 0);
}

void release(JNIEnv *env, jobject instance) {
    rtmp_destroy();
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
        {"setAudiosamplesize", "(D)V",                  (void *) setAudiosamplesize}
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
    if ((*vm)->GetEnv(vm, (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        SRS_LOGE("Env not got");
        return JNI_ERR;
    }

    jclass clz = (*jenv)->FindClass(jenv, JNI_CLS_MANAGER);
    if (clz == NULL) {
        SRS_LOGE("JNI_OnLoad:Class %s not found", JNI_CLS_MANAGER);
        return JNI_ERR;
    }

    if ((*jenv)->RegisterNatives(jenv, clz, srs_methods, SRS_ARRAY_ELEMS(srs_methods))) {
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
    if ((*vm)->GetEnv(vm, (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        SRS_LOGE("Env not got");
        return;
    }
    jclass clz = (*jenv)->FindClass(jenv, JNI_CLS_MANAGER);
    if (clz == NULL) {
        SRS_LOGE("JNI_OnUnload:Class %s not found", JNI_CLS_MANAGER);
        return;
    }
    (*jenv)->UnregisterNatives(jenv, clz);
}