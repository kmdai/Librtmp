//
// Created by kmdai on 18-4-19.
//

#include <jni.h>
#include "push_rtmp.h"
#include "push_flvenc.h"
#include <sys/prctl.h>

RTMP *m_pRtmp;
media_config *media_config_p = NULL;
bool mate_data = false;

int init_srs(const char *url) {
    init_queue();
    media_config_p = (media_config *) malloc(sizeof(media_config));
    memset(media_config_p, 0, sizeof(media_config));
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
    if (media_config_p != NULL) {
        free(media_config_p);
    }
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
        if (!mate_data) {
            mate_data = true;
            q_node_p node = create_node(data, 0,
                                        NODE_TYPE_METADATA, NODE_TYPE_METADATA, 0);
            in_queue(node);
        }
        if (node_p->type == NODE_TYPE_METADATA) {
            packet_type = RTMP_PACKET_TYPE_INFO;
            header_type = RTMP_PACKET_SIZE_MEDIUM;
            channel = 0x03;
            size = create_MetaData(&data, media_config_p->frame_rate,
                                   media_config_p->video_bit_rate / 1000.0,
                                   7,
                                   media_config_p->width,
                                   media_config_p->height,
                                   10,
                                   media_config_p->audio_bit_rate / 1000.0,
                                   media_config_p->audio_sample_rate,
                                   16.0,
                                   0);
            SRS_LOGE(
                    "NODE_TYPE_METADATA:frame_rate:%d,video_bit_rate:%d,video_id:%d,width:%d,height:%d,audio_id:%d,audio_bit_rate;%d,audio_sample_rate:%d,bit_size;%d",
                    media_config_p->frame_rate,
                    media_config_p->video_bit_rate,
                    7,
                    media_config_p->width,
                    media_config_p->height,
                    10,
                    media_config_p->audio_bit_rate,
                    media_config_p->audio_sample_rate,
                    16,
                    0);
            SRS_LOGE("NODE_TYPE_METADATA:time;%ld", node_p->time);
        } else if (node_p->type == NODE_TYPE_AUDIO) {
            packet_type = RTMP_PACKET_TYPE_AUDIO;
            channel = 0x05;
            if (node_p->flag == NODE_FLAG_CODEC_CONFIG) {
                header_type = RTMP_PACKET_SIZE_MEDIUM;
                size = create_AACSequenceHeader(&data, node_p->data, node_p->size);
//                SRS_LOGE("NODE_FLAG_CODEC_CONFIG----%2x,%2x", node_p->data[0], node_p->data[1]);
//                SRS_LOGE("NODE_FLAG_CODEC_CONFIG-data----%2x,%2x,%2x,%2x,%2x,%2x,%2x", data[0],
//                         data[1], data[2], data[3], data[4], data[5], data[6]);
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
            if (!RTMP_SendPacket(m_pRtmp, &packet, TRUE)) {
                SRS_LOGE("---%s", "send__false");
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

void set_config(media_config *config) {
    media_config_p = config;
}

void rtmp_start(JavaVM *gVm) {
    pthread_t pthread;
    int result = pthread_create(&pthread, NULL, push_data, gVm);
    if (result < 0) {
        SRS_LOGE("pthread_create false");
    }
    pthread_setname_np(pthread, "rtmp push data");
}

void set_framerate(uint32_t framerate) {
    if (media_config_p) {
        media_config_p->frame_rate = framerate;
    }
}

void set_VideoBitrate(uint32_t videodatarate) {
    if (media_config_p) {
        media_config_p->video_bit_rate = videodatarate;
    }
}

void set_Width(uint32_t width) {
    if (media_config_p) {
        media_config_p->width = width;
    }
}

void set_Height(uint32_t height) {
    if (media_config_p) {
        media_config_p->height = height;
    }
}

void set_AudioBitrate(uint32_t audiodatarate) {
    if (media_config_p) {
        media_config_p->audio_bit_rate = audiodatarate;
    }
}

void set_Channel(int32_t channel) {
    if (media_config_p) {
        media_config_p->channel_count = channel;
    }
}

void set_Samplerate(uint32_t audiosamplerate) {
    if (media_config_p) {
        media_config_p->audio_sample_rate = audiosamplerate;
    }
}




