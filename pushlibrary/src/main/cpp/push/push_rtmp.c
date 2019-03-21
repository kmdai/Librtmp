//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push_rtmp.h"
#include "push_flvenc.h"
#include <sys/prctl.h>

RTMP *m_pRtmp;
media_config *media_config_p;

int init_srs(const char *url) {
    media_config_p = (media_config *) malloc(sizeof(media_config));
    init_queue();
    m_pRtmp = RTMP_Alloc();
    if (m_pRtmp) {
        RTMP_Init(m_pRtmp);
        m_pRtmp->Link.timeout = 1000;
    } else {
        SRS_LOGE("rtmp_init %s", "--------ERROR");
    }
    if (RTMP_SetupURL(m_pRtmp, url) == TRUE) {
        RTMP_EnableWrite(m_pRtmp);
        if (RTMP_Connect(m_pRtmp, NULL) == FALSE) {
            rtmp_destroy();
            SRS_LOGE("RTMP_Connect----FALSE %s", url);
            return FALSE;
        }
        if (RTMP_ConnectStream(m_pRtmp, 0) == FALSE) {
            SRS_LOGE("RTMP_ConnectStream----FALSE %s", url);
            rtmp_destroy();
            return FALSE;
        }
        SRS_LOGE("set_Connect----TRUE %s", url);
        return TRUE;
    }
    SRS_LOGE("set_Connect----FALSE%s", url);
    return FALSE;
}

void rtmp_destroy() {
    free(media_config_p);
    cancel_queue();
//    if (m_pRtmp) {
//        //关闭rtmp
//        RTMP_Close(m_pRtmp);
//        //释放
//        RTMP_Free(m_pRtmp);
//    }
    //销毁队列
//    destroy_queue();
}

void write_raw_frames(char *data, int32_t size, u_int32_t dts, u_int32_t pts) {
    if (NULL == m_pRtmp) {
        return;
    }
}

void add_frame(char *data, int32_t size, int32_t type, uint32_t time) {
}


void *push_data(void *gVm) {
    JavaVM *gvm = (JavaVM *) gVm;
    JNIEnv *env = NULL;
    prctl(PR_SET_NAME, "rtmp push data");
    if (0 != (*gvm)->AttachCurrentThread(gVm, &env, NULL)) {
        SRS_LOGE("----%s", "AttachCurrentThread FALSE");
        return (void *) 0;
    }
    for (;;) {
        q_node_p node_p = out_queue();
        if (NULL == node_p) {
            break;
        }
        RTMPPacket packet;
        uint8_t packet_type;
        uint8_t header_type;
        int size = 0;
        int channel;
        char *data;
        if (node_p->type == NODE_TYPE_AUDIO) {
            packet_type = RTMP_PACKET_TYPE_AUDIO;
            channel = 0x04;
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                header_type = RTMP_PACKET_SIZE_MEDIUM;
                size = create_AACSequenceHeader(&data, 0, 0);
            } else {
                header_type = RTMP_PACKET_SIZE_LARGE;
                size = create_AudioPacket(&data, node_p->data, node_p->flag, node_p->size, 0);
            }
        } else {
            packet_type = RTMP_PACKET_TYPE_VIDEO;
            channel = 0x04;
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                header_type = RTMP_PACKET_SIZE_MEDIUM;
                size = create_AVCVideoPacket(&data, node_p->data, node_p->size);
            } else {
                header_type = RTMP_PACKET_SIZE_LARGE;
                size = create_VideoPacket(&data, node_p->data, node_p->flag, node_p->size, 0);
            }

        }
        RTMPPacket_Alloc(&packet, size);
        RTMPPacket_Reset(&packet);
        memcpy(packet.m_body, data, size);
        packet.m_packetType = packet_type;
        packet.m_nBodySize = size;
        packet.m_nChannel = channel;
        packet.m_nTimeStamp = node_p->time;
        packet.m_hasAbsTimestamp = 0;
        packet.m_headerType = header_type;
        packet.m_nInfoField2 = m_pRtmp->m_stream_id;
        /*发送*/
        if (RTMP_IsConnected(m_pRtmp)) {
            if (RTMP_SendPacket(m_pRtmp, &packet, TRUE)) {
                SRS_LOGE("---%s", "send_sps_pps_true");
            }
        }
        RTMPPacket_Free(&packet);
        free(data);
        free(node_p);
    }
    //关闭rtmp
    RTMP_Close(m_pRtmp);
    //释放
    RTMP_Free(m_pRtmp);
    SRS_LOGE("node_p=NULL,DetachCurrentThread");
    (*gvm)->DetachCurrentThread(gvm);
    return (void *) 1;
}

void rtmp_start(JavaVM *gVm) {
    pthread_t pthread;
    int result = pthread_create(&pthread, NULL, push_data, gVm);
    if (!result) {
        SRS_LOGE("pthread_create false");
    }
    pthread_setname_np(pthread, "rtmp push data");
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

void set_audiochannel(int32_t channel) {
    if (media_config_p) {
        media_config_p->channel_count = channel;
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


