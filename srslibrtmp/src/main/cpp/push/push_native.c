//
// Created by kmdai on 18-4-19.
//

#include "push_rtmp.h"

#define SRS_ARRAY_ELEMS(a)  (sizeof(a) / sizeof(a[0]))

static JavaVM *javaVM;

jboolean setUrl(JNIEnv *env, jobject instance, jstring url) {
    const char *rtmp_url = (*env)->GetStringUTFChars(env, url, 0);
    int result = init_srs(rtmp_url);
    if (result == 0) {
        rtmp_start(javaVM);
    }
    (*env)->ReleaseStringUTFChars(env, url, rtmp_url);
    return result != 0 ? JNI_FALSE : JNI_TRUE;
}

void addFrame(JNIEnv *env, jobject instance, jbyteArray data, jint size, jint type, jint time) {
    jbyte *chunk = (*env)->GetByteArrayElements(env, data, NULL);
    q_node_p node = create_node(chunk, size, type, time);
    in_queue(node);
    (*env)->ReleaseByteArrayElements(env, data, chunk, 0);
}

void release(JNIEnv *env, jobject instance) {
    rtmp_destroy();
}

/**
 * 本地函数
 */
const JNINativeMethod srs_methods[] = {
        {"setUrl",   "(Ljava/lang/String;)Z", (void *) setUrl},
        {"release",  "()V",                   (void *) release},
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