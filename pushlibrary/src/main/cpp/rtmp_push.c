//
// Created by kmdai on 18-1-24.
//
#include "rtmp_push.h"
#include <malloc.h>
#include "librtmp/log.h"


/**
 * 设置url
 * @param r
 * @param url
 */
int set_Connect(char *url) {
    if (RTMP_SetupURL(m_pRtmp, url) == TRUE) {
        RTMP_EnableWrite(m_pRtmp);
        if (RTMP_Connect(m_pRtmp, NULL) == FALSE) {
            rtmp_free();
            LOG_V("RTMP_Connect----FALSE", "%s", url);
            return FALSE;
        }
        if (RTMP_ConnectStream(m_pRtmp, 0) == FALSE) {
            LOG_V("RTMP_ConnectStream----FALSE", "%s", url);
            rtmp_free();
            return FALSE;
        }
        LOG_V("set_Connect----TRUE", "%s", url);
        return TRUE;
    }
    LOG_V("set_Connect----FALSE", "%s", url);
    return FALSE;
}


/**
 * 初始化rtmp
 * @return
 */
void rtmp_init() {
    RTMP_LogSetLevel(RTMP_LOGDEBUG);
    m_pRtmp = RTMP_Alloc();
    if (m_pRtmp) {
        LOG_V("rtmp_init", "%s", "--------");
        RTMP_Init(m_pRtmp);
        m_pRtmp->Link.timeout = 1000;
    } else {
        LOG_V("rtmp_init", "%s", "--------ERROR");
    }
}

void rtmp_free() {
    LOG_V("rtmp_init", "%s", "----rtmp_free");
    //关闭rtmp
    RTMP_Close(m_pRtmp);
    //释放
    RTMP_Free(m_pRtmp);
    if (m_pSPS_PPS) {
        free(m_pSPS_PPS);
    }
}

/**
 * 读取sps|pps数据
 * @param nalu_sps
 * @param data
 * @return
 */
int read_nalu_sps_pps(NaluUnit *nalu_sps, NaluUnit *nalu_pps, uint8_t *data, int32_t size,
                      int32_t offset) {

    return 1;
}

int send_sps_pps() {

    int spsLen = m_pSPS_PPS->nSpsLen;
    int ppsLen = m_pSPS_PPS->nPpsLen;
    uint8_t *sps = m_pSPS_PPS->Sps;
    uint8_t *pps = m_pSPS_PPS->Pps;
    RTMPPacket packet;
    RTMPPacket_Alloc(&packet, SPS_PPS_HEAD_SIZE + spsLen + ppsLen);
    RTMPPacket_Reset(&packet);
    char *body = packet.m_body;

    int i = 0;

    body[i++] = 0x17;
    body[i++] = 0x00;

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    /**
     *AVCDecoderConfigurationRecord
     */
    body[i++] = 0x01;
    body[i++] = sps[1];
    body[i++] = sps[2];
    body[i++] = sps[3];
    body[i++] = 0xff;

    /*sps*/
    body[i++] = 0xe1;
    body[i++] = (spsLen >> 8) & 0xff;
    body[i++] = spsLen & 0xff;

    memcpy(&body[i], sps, spsLen);

    i += spsLen;

    /*pps*/
    body[i++] = 0x01;
    body[i++] = (ppsLen >> 8) & 0xff;
    body[i++] = ppsLen & 0xff;
    memcpy(&body[i], pps, ppsLen);

    i += ppsLen;

    packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet.m_nBodySize = i;
    packet.m_nChannel = 0x04;
    packet.m_nTimeStamp = 0;
    packet.m_hasAbsTimestamp = 0;
    packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;

    if (RTMP_IsConnected(m_pRtmp)) {
        if (RTMP_SendPacket(m_pRtmp, &packet, TRUE)) {
            LOG_V("rtmp---", "%s", "send_sps_pps---TRUE");
            RTMPPacket_Free(&packet);
            return TRUE;
        }
    }
    RTMPPacket_Free(&packet);
    LOG_V("rtmp---", "%s", "send_sps_pps---FALSE");
    return FALSE;
}

int send_rtmp_packet(NaluUnit *naluUnit, int keyFrame, uint32_t timeStamp, int add_queue) {

    RTMPPacket packet;
    int body_size = naluUnit->size + 9;
    RTMPPacket_Alloc(&packet, body_size);
    RTMPPacket_Reset(&packet);
    char *body = packet.m_body;
    memset(body, 0, naluUnit->size + 9);
//    for(int i=0;i<4;i++){
//        LOG_V("---","%x",naluUnit->data[i]);
//    }
    int type;
    type = naluUnit->data[0] & 0x1f;

    int i = 0;
    //默认是非关键帧(2:Pframe  7:AVC)
    body[i++] = 0x27;
    body[i++] = 0x01;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    body[i++] = (naluUnit->size >> 24) & 0xff;
    body[i++] = (naluUnit->size >> 16) & 0xff;
    body[i++] = (naluUnit->size >> 8) & 0xff;
    body[i++] = naluUnit->size & 0xff;

    if (type == 5) {
        //关键帧1:Iframe  7:AVC
        body[0] = 0x17;
    }
    memcpy(&body[i], naluUnit->data, naluUnit->size);

    packet.m_nBodySize = body_size;
    packet.m_hasAbsTimestamp = 0;
    packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;
    packet.m_nChannel = 0x04;
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet.m_nTimeStamp = timeStamp;
    int ret = 0;
    if (RTMP_IsConnected(m_pRtmp)) {
        ret = RTMP_SendPacket(m_pRtmp, &packet, TRUE);
        if (ret) {
            LOG_V("----", "%s", "RTMP_SendPacket_TRUE");
        } else {
            LOG_V("----", "%s", "RTMP_SendPacket_FALSE");
        }
        LOG_V("----RTMP_IsConnected", "%d", timeStamp);
    } else {
        LOG_V("----", "%s", "send_rtmp_packet_RTMP_IsConnected_FALSE");
    }
    RTMPPacket_Free(&packet);
    return ret;
}

void set_sps_pps(uint8_t *data, uint32_t size) {
    NaluUnit *sps_nalu;
    NaluUnit *pps_nalu;

    for (int i = 0; i < size; i++) {
        LOG_V("data-----:", "%x", data[i]);
    }
    int32_t nalu_pos = 0;
    //sps数据起始位
    int32_t sps_pos = find_sps_pps_pos(data, size, nalu_pos);
    LOG_V("read_nalu_sps_pps----", "%d", sps_pos);
    int32_t pps_pos = find_sps_pps_pos(data, size, sps_pos);
    LOG_V("read_nalu_pps----", "%d", pps_pos);

    int32_t sps_size = pps_pos - sps_pos - SPS_PPS_DIVIDE;
    sps_nalu = (NaluUnit *) malloc(sizeof(NaluUnit) + sps_size);
    sps_nalu->data = (uint8_t *) sps_nalu + sizeof(NaluUnit);
    sps_nalu->size = sps_size;

    memcpy(sps_nalu->data, data + sps_pos, sps_nalu->size);
//    for (int i = 0; i < sps_nalu->size; i++) {
//        LOG_V("sps->data-----:", "%x", sps_nalu->data[i]);
//    }
    sps_nalu->type = sps_nalu->data[0] & 0x1f;

    int32_t pps_size = size - pps_pos;
    pps_nalu = (NaluUnit *) malloc(sizeof(NaluUnit) + pps_size);
    pps_nalu->size = pps_size;
    pps_nalu->data = (uint8_t *) pps_nalu + sizeof(NaluUnit);
    memcpy(pps_nalu->data, data + pps_pos, (size_t) pps_nalu->size);
//    for (int i = 0; i < pps_nalu->size; i++) {
//        LOG_V("pps->data-----:", "%x", pps_nalu->data[i]);
//    }
    //释放之前保留的信息
//    if (m_pSPS_PPS) {
//        free(m_pSPS_PPS);
//    }
    m_pSPS_PPS = (RTMPMetadata *) malloc(sizeof(RTMPMetadata) + sps_nalu->size + pps_nalu->size);
    m_pSPS_PPS->Sps = (uint8_t *) m_pSPS_PPS + sizeof(RTMPMetadata);
    m_pSPS_PPS->nSpsLen = sps_nalu->size;
    m_pSPS_PPS->Pps = (uint8_t *) m_pSPS_PPS + sizeof(RTMPMetadata) + sps_nalu->size;
    m_pSPS_PPS->nPpsLen = pps_nalu->size;
    memcpy(m_pSPS_PPS->Sps, sps_nalu->data, sps_nalu->size);
    memcpy(m_pSPS_PPS->Pps, pps_nalu->data, pps_nalu->size);
    send_sps_pps();
//    sendSpsAndPps(m_pSPS_PPS->Sps,m_pSPS_PPS->nSpsLen,m_pSPS_PPS->Pps,m_pSPS_PPS->nPpsLen,0);
    free(sps_nalu);
    free(pps_nalu);
}

int sendSpsAndPps(uint8_t *sps, int spsLen, uint8_t *pps, int ppsLen, long timestamp) {
//    sps = m_pSPS_PPS->Sps;
//    pps = m_pSPS_PPS->Pps;
//    spsLen = m_pSPS_PPS->nSpsLen;
//    ppsLen = m_pSPS_PPS->nPpsLen;
    int i;
//    RTMPPacket *packet = (RTMPPacket *) malloc(RTMP_HEAD_SIZE + 16 + spsLen + ppsLen);
    RTMPPacket packet;
    RTMPPacket_Alloc(&packet, SPS_PPS_HEAD_SIZE + spsLen + ppsLen);
    RTMPPacket_Reset(&packet);
//    memset(packet, 0, RTMP_HEAD_SIZE);
//    packet.m_body = (char *) &packet + RTMP_HEAD_SIZE;
    char *body = packet.m_body;

    i = 0;
    body[i++] = 0x17; //1:keyframe 7:AVC
    body[i++] = 0x00; // AVC sequence header

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00; //fill in 0

    /*AVCDecoderConfigurationRecord*/
    body[i++] = 0x01;
    body[i++] = sps[1]; //AVCProfileIndecation
    body[i++] = sps[2]; //profile_compatibilty
    body[i++] = sps[3]; //AVCLevelIndication
    body[i++] = 0xff;//lengthSizeMinusOne

    /*SPS*/
    body[i++] = 0xe1;
    body[i++] = (spsLen >> 8) & 0xff;
    body[i++] = spsLen & 0xff;
    /*sps data*/
    memcpy(&body[i], sps, spsLen);

    i += spsLen;

    /*PPS*/
    body[i++] = 0x01;
    /*sps data length*/
    body[i++] = (ppsLen >> 8) & 0xff;
    body[i++] = ppsLen & 0xff;
    memcpy(&body[i], pps, ppsLen);
    i += ppsLen;
//    for (int j = 0; j < i; j++) {
//        LOGD("%x", packet->m_body[j]);
//    }
    packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet.m_nBodySize = i;
    packet.m_nChannel = 0x04;
    packet.m_nTimeStamp = 0;
    packet.m_hasAbsTimestamp = 0;
    packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;

    /*发送*/
    if (RTMP_IsConnected(m_pRtmp)) {
       if( RTMP_SendPacket(m_pRtmp, &packet, TRUE)){
           LOG_V("---","%s","send_sps_pps_true");
       }
    }
    RTMPPacket_Free(&packet);
//    free(packet);
    return 0;
}