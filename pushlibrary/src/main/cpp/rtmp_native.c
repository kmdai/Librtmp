//
// Created by kmdai on 18-1-25.
//

#include <jni.h>
#include "rtmp_push.h"
#include <malloc.h>

JNIEXPORT jboolean JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_rtmpInit(JNIEnv *env, jobject instance) {

    rtmp_init();
}

JNIEXPORT jboolean JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_rtmpFree(JNIEnv *env, jobject instance) {
    rtmp_free();
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_sendChunk(JNIEnv *env, jobject instance, jbyteArray chunk_,
                                                 jint size, jint keyframe, jlong timestamp) {
    jbyte *chunk = (*env)->GetByteArrayElements(env, chunk_, NULL);

    NaluUnit *naluUnit = (NaluUnit *) malloc(sizeof(NaluUnit) + size);
    naluUnit->data = (uint8_t *) naluUnit + sizeof(NaluUnit);
    naluUnit->size = size;
    memcpy(naluUnit->data, chunk, size);
    send_rtmp_packet(naluUnit, keyframe, timestamp, 1);
    free(naluUnit);
    (*env)->ReleaseByteArrayElements(env, chunk_, chunk, 0);
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_setUrl(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);

    set_Connect(url);

    (*env)->ReleaseStringUTFChars(env, url_, url);
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_setSpsPps(JNIEnv *env, jobject instance, jbyteArray data_,
                                                 jint size) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    set_sps_pps(data, size);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_sendSpsPPs(JNIEnv *env, jobject instance, jbyteArray sps_,
                                                  jint spsLen, jbyteArray pps_, jint ppsLen) {
    jbyte *sps = (*env)->GetByteArrayElements(env, sps_, NULL);
    jbyte *pps = (*env)->GetByteArrayElements(env, pps_, NULL);

    sendSpsAndPps(sps, spsLen, pps, ppsLen, 0);

    (*env)->ReleaseByteArrayElements(env, sps_, sps, 0);
    (*env)->ReleaseByteArrayElements(env, pps_, pps, 0);
}
