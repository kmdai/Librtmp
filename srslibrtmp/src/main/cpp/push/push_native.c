//
// Created by kmdai on 18-4-19.
//

#include "push_rtmp.h"

#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))

static JavaVM *javaVM;

void setUrl(JNIEnv *env, jstring url) {
    const char *rtmp_url = (*env)->GetStringUTFChars(env, url, NULL);
    init_srs(rtmp_url);
    (*env)->ReleaseStringUTFChars(env, url, rtmp_url);
}

void addFrame(JNIEnv *env, jbyteArray data, jint size, jint type, jint time) {
    jbyte *chunk = (*env)->GetByteArrayElements(env, data, NULL);
    q_node_p node = create_node(chunk, size, type, time);
    in_queue(node);
    (*env)->ReleaseByteArrayElements(env, chunk, chunk, 0);
}

/**
 * 本地函数
 */
const JNINativeMethod srs_methods[] = {
        {"setUrl",   "(Ljava/lang/String;)V", (void *) setUrl},
        {"addFrame", "([BIII)V",              (void *) addFrame}
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
    jclass clz = (*jenv)->FindClass(jenv, "com/kmdai/srslibrtmp/SRSLibrtmpManager");
    if (clz == NULL) {
        SRS_LOGE("Class \"com/kmdai/srslibrtmp/SRSLibrtmpManager\" not found");
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
    jclass clz = (*jenv)->FindClass(jenv, "com/kmdai/srslibrtmp/SRSLibrtmpManager");
    if (clz == NULL) {
        SRS_LOGE("Class \"com/kmdai/srslibrtmp/SRSLibrtmpManager\" not found");
        return;
    }
    (*jenv)->UnregisterNatives(jenv, clz);
}