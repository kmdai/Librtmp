//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push_rtmp.h"

srs_rtmp_t srs_rtmp;

int init_srs(const char *url) {
    srs_rtmp = srs_rtmp_create(url);
    init_queue();
    if (srs_rtmp_handshake(srs_rtmp) != 0) {
        rtmp_destroy();
        return 1;
    }
    if (srs_rtmp_connect_app(srs_rtmp) != 0) {
        SRS_LOGE("connect vhost/app failed.");
        rtmp_destroy();
        return 1;
    }

    if (srs_rtmp_publish_stream(srs_rtmp) != 0) {
        SRS_LOGE("publish stream failed.");
        rtmp_destroy();
        return 1;
    }
    return 0;
}

void rtmp_destroy() {
    cancel_queue();
    //销毁队列
    destroy_queue();
}

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts) {
    if (NULL == srs_rtmp) {
        return;
    }
    srs_h264_write_raw_frames(srs_rtmp, data, size, dts, pts);
}

void add_frame(char *data, int32_t size, int32_t type, uint32_t time) {
    q_node_p node_p = create_node(data, size, type, time);
    in_queue(node_p);
}

void *push_data(void *gVm) {
    JavaVM *gvm = (JavaVM *) gVm;
    JNIEnv *env = NULL;
    if (0 != (*gvm)->AttachCurrentThread(gVm, &env, NULL)) {
        return (void *) 0;
    }
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            SRS_LOGE("node_p=NULL");
            (*gvm)->DetachCurrentThread(gvm);
            return (void *) 1;
        }
//        SRS_LOGE("push rtmp frame-%d", node_p->size);
        write_raw_frames(node_p->data, node_p->size, node_p->time, node_p->time);
        free(node_p);
    }
}

void rtmp_start(JavaVM *gVm) {
    pthread_t pthread;
    int result = pthread_create(&pthread, NULL, push_data, gVm);
}
