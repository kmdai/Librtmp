//
// Created by kmdai on 18-1-25.
//

#include <jni.h>
#include "rtmp_push.h"

JNIEXPORT jboolean JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_rtmpInit(JNIEnv *env, jobject instance) {

    // TODO

}

JNIEXPORT jboolean JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_rtmpFree(JNIEnv *env, jobject instance) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_sendChunk(JNIEnv *env, jobject instance, jbyteArray chunk_) {
    jbyte *chunk = (*env)->GetByteArrayElements(env, chunk_, NULL);

    // TODO

    (*env)->ReleaseByteArrayElements(env, chunk_, chunk, 0);
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_setUrl(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = (*env)->GetStringUTFChars(env, url_, 0);

    // TODO

    (*env)->ReleaseStringUTFChars(env, url_, url);
}

JNIEXPORT void JNICALL
Java_com_kmdai_rtmppush_LibrtmpManager_setSpsPps(JNIEnv *env, jobject instance, jbyteArray data_,
                                                 jint size) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
    // TODO
    read_nalu_sps_pps(NULL, NULL, data, size, 0);
    (*env)->ReleaseByteArrayElements(env, data_, data, 0);
}

