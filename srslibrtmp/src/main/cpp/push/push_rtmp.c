//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push_rtmp.h"
#include "push_flvenc.h"
#include <sys/prctl.h>

srs_rtmp_t srs_rtmp;

int init_srs(const char *url) {
    srs_rtmp = srs_rtmp_create(url);
    media_config_p = (media_config *) malloc(sizeof(media_config));
    init_queue();
    int ret;
    if ((ret = srs_rtmp_handshake(srs_rtmp)) != 0) {
        SRS_LOGE("srs_rtmp_handshake failed.:%d", ret);
        rtmp_destroy();
        return 0;
    }
    if (srs_rtmp_connect_app(srs_rtmp) != 0) {
        SRS_LOGE("connect vhost/app failed.");
        rtmp_destroy();
        return 0;
    }

    if (srs_rtmp_publish_stream(srs_rtmp) != 0) {
        SRS_LOGE("publish stream failed.");
        rtmp_destroy();
        return 0;
    }
    return 1;
}

void rtmp_destroy() {
    free(media_config_p);
    cancel_queue();
    //销毁队列
//    destroy_queue();
}

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts) {
    if (NULL == srs_rtmp) {
        return;
    }
    srs_h264_write_raw_frames(srs_rtmp, data, size, dts, pts);
}

void add_frame(char *data, int32_t size, int32_t type, uint32_t time) {
//    q_node_p node_p = create_node(data, size, type, time);
//    in_queue(node_p);
}


void *push_data(void *gVm) {
    JavaVM *gvm = (JavaVM *) gVm;
    JNIEnv *env = NULL;
    prctl(PR_SET_NAME, "rtmp push data");
    if (0 != (*gvm)->AttachCurrentThread(gVm, &env, NULL)) {
        return (void *) 0;
    }
    int ret;
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            break;
        }
        if (node_p->type == NODE_TYPE_AUDIO) {
            char *data;
            int size = 0;
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                size = create_AACSequenceHeader(&data, node_p->data, node_p->size);
            } else {
                size = create_AudioPacket(&data, node_p->data, node_p->flag, node_p->size, 0);
            }
            if ((ret = srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_AUDIO, node_p->time, data,
                                             size)) != 0) {
                SRS_LOGE("srs_audio_write_raw_frame error%d", ret);
            }
        } else if (node_p->type == NODE_TYPE_VIDEO) {
            char *data;
            int size = 0;
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                size = create_AVCVideoPacket(&data, node_p->data, node_p->size);
                srs_human_print_rtmp_packet(SRS_RTMP_TYPE_VIDEO, node_p->time, data, size);
            } else {
                size = create_VideoPacket(&data, node_p->data, node_p->flag, node_p->size, 0);
                srs_human_print_rtmp_packet(SRS_RTMP_TYPE_VIDEO, node_p->time, data, size);
            }
            if ((ret = srs_rtmp_write_packet(srs_rtmp, SRS_RTMP_TYPE_VIDEO, node_p->time, data,
                                             size)) !=
                0) {
                SRS_LOGE("srs_rtmp_write_packet fail:%d ", ret);
            }
        }
        free(node_p);
    }
    srs_rtmp_destroy(srs_rtmp);
    SRS_LOGE("node_p=NULL,DetachCurrentThread");
    (*gvm)->DetachCurrentThread(gvm);
    return (void *) 1;
}

void rtmp_start(JavaVM *gVm) {
    pthread_t pthread;
    int result = pthread_create(&pthread, NULL, push_data, gVm);
    pthread_setname_np(pthread, "rtmp push data");
}

void set_framerate(uint32_t framerate) {
    if (media_config_p) {
        media_config_p->framerate = framerate;
    }
}

void set_videodatarate(uint32_t videodatarate) {
    if (media_config_p) {
        media_config_p->videodatarate = videodatarate;
    }
}

void set_width(uint32_t width) {
    if (media_config_p) {
        media_config_p->width = width;
    }
}

void set_height(uint32_t height) {
    if (media_config_p) {
        media_config_p->height = height;
    }
}

void set_audiodatarate(uint32_t audiodatarate) {
    if (media_config_p) {
        media_config_p->audiodatarate = audiodatarate;
    }
}

void set_audiochannel(int32_t channel) {
    if (media_config_p) {
        media_config_p->channel_count = channel;
    }
}

void set_audiosamplerate(uint32_t audiosamplerate) {
    if (media_config_p) {
        media_config_p->audiosamplerate = audiosamplerate;
    }
}

void set_audiosamplesize(uint32_t audiosamplesize) {
    if (media_config_p) {
        media_config_p->audiosamplesize = audiosamplesize;
    }
}


