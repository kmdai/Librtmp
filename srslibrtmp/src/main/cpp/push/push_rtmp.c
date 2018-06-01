//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push_rtmp.h"
#include "push_flvenc.h"

srs_rtmp_t srs_rtmp;
media_config *media_config_p;

int init_srs(const char *url) {
    srs_rtmp = srs_rtmp_create(url);
    media_config_p = (media_config *) malloc(sizeof(media_config));
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
    free(media_config_p);
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
    int ret;
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            SRS_LOGE("node_p=NULL");
            (*gvm)->DetachCurrentThread(gvm);
            return (void *) 1;
        }
//        SRS_LOGE("push rtmp frame-%d", node_p->size);
        //sps、pps
        if (node_p->type == NODE_FLAG_CODEC_CONFIG) {
            if (media_config_p) {
                char *meta;
                int metaSize = create_MetaData(&meta,
                                               media_config_p->framerate,
                                               media_config_p->videodatarate,
                                               7,
                                               media_config_p->width,
                                               media_config_p->height,
                                               10,
                                               media_config_p->audiodatarate,
                                               media_config_p->audiosamplerate,
                                               media_config_p->audiosamplesize,
                                               1);
                SRS_LOGE("meta.len=%d", (int) strlen(meta));
                srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_SCRIPT, 0, meta, metaSize);
            }
            char *data;
            int size = create_AVCVideoPacket(&data, node_p->data, node_p->size);
            srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_VIDEO, 0, data, size);
//            free(data);
        } else {
            char *data;
            int size = create_VideoPacket(&data, node_p->data, node_p->type, node_p->size, 0);
            if ((ret = srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_VIDEO, node_p->time, data,
                                             size)) !=
                0) {
                SRS_LOGE("srs_rtmp_write_packet fail:%d ", ret);
            }
//            free(data);
        }
//        char *data=(char*)malloc(node_p->size);
//        memcpy(data,node_p->data,node_p->size);
//        if(srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_VIDEO, node_p->time, data, node_p->size)!=0){
//            SRS_LOGE("srs_rtmp_write_packet fail:%d ", 0);
//        }
//        srs_h264_write_raw_frames(srs_rtmp,node_p->data,node_p->size,node_p->time,node_p->time);
        free(node_p);
    }
}

void rtmp_start(JavaVM *gVm) {
    pthread_t pthread;
    int result = pthread_create(&pthread, NULL, push_data, gVm);
}

void set_framerate(double framerate) {
    if (media_config_p) {
        media_config_p->framerate = framerate;
    }
}

void set_videodatarate(double videodatarate) {
    if (media_config_p) {
        media_config_p->videodatarate = videodatarate;
    }
}

void set_width(double width) {
    if (media_config_p) {
        media_config_p->width = width;
    }
}

void set_height(double height) {
    if (media_config_p) {
        media_config_p->height = height;
    }
}

void set_audiodatarate(double audiodatarate) {
    if (media_config_p) {
        media_config_p->audiodatarate = audiodatarate;
    }
}

void set_audiosamplerate(double audiosamplerate) {
    if (media_config_p) {
        media_config_p->audiosamplerate = audiosamplerate;
    }
}

void set_audiosamplesize(double audiosamplesize) {
    if (media_config_p) {
        media_config_p->audiosamplesize = audiosamplesize;
    }
}


