//
// Created by kmdai on 18-1-24.
//
#include "rtmp_push.h"
#include <malloc.h>


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
    m_pRtmp = RTMP_Alloc();
    if (m_pRtmp) {
        LOG_V("rtmp_init", "%s", "--------");
        RTMP_Init(m_pRtmp);
    } else {
        LOG_V("rtmp_init", "%s", "--------ERROR");
    }
}

void rtmp_free() {
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
    if (RTMP_SendPacket(m_pRtmp, m_pSPS_PPS, TRUE)) {
        LOG_V("rtmp---", "%s", "send_sps_pps---TRUE");
        return TRUE;
    }
    LOG_V("rtmp---", "%s", "send_sps_pps---FALSE");
    return FALSE;
}

int send_rtmp_packet(NaluUnit *naluUnit, int keyFrame, uint32_t timeStamp, int add_queue) {
    LOG_V("send_rtmp_packet---", "send_rtmp_packet----%d", naluUnit->size);
    for (int i = 0; i < 5; i++) {
        LOG_V("send_rtmp_packet-naluUnit->data", "%x", naluUnit->data[i]);
    }
    uint8_t *body = (uint8_t *) malloc(naluUnit->size + 9);
    int i = 1;
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
    if (keyFrame) {
        LOG_V("keyFrame---", "%s", "send_rtmp_packet---keyFrame");
        //关键帧1:Iframe  7:AVC
        body[0] = 0x17;
        send_sps_pps();
    }
    memcpy(&body[i], naluUnit->data, naluUnit->size);

    int body_size = i + naluUnit->size;
    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, body_size);
    packet.m_nBodySize = naluUnit->size;
    memcpy(packet.m_body, body, body_size);
    packet.m_hasAbsTimestamp = 0;
    packet.m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;
    packet.m_nChannel = 0x04;

    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet.m_nTimeStamp = timeStamp;
    int ret = 0;
    if (RTMP_IsConnected(m_pRtmp)) {
        ret = RTMP_SendPacket(m_pRtmp, &packet, TRUE);
        LOG_V("RTMP_SendPacket----", "%s", "------");
    }
    RTMPPacket_Free(&packet);
    free(body);
//    free(naluUnit);
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
    for (int i = 0; i < sps_nalu->size; i++) {
        LOG_V("sps->data-----:", "%x", sps_nalu->data[i]);
    }
    sps_nalu->type = sps_nalu->data[0] & 0x1f;

    int32_t pps_size = size - pps_pos;
    pps_nalu = (NaluUnit *) malloc(sizeof(NaluUnit) + pps_size);
    pps_nalu->size = pps_size;
    pps_nalu->data = (uint8_t *) pps_nalu + sizeof(NaluUnit);
    LOG_V("read_nalu_pps----", "%d", pps_pos);
    memcpy(pps_nalu->data, data + pps_pos, (size_t) pps_nalu->size);
    for (int i = 0; i < pps_nalu->size; i++) {
        LOG_V("pps->data-----:", "%x", pps_nalu->data[i]);
    }
    int body_size = sps_nalu->size + pps_nalu->size + SPS_PPS_HEAD_SIZE;
    //释放之前保留的信息
    if (m_pSPS_PPS) {
        free(m_pSPS_PPS);
    }
    m_pSPS_PPS = (RTMPPacket *) malloc(RTMP_PACKET_SIZE + body_size);
    m_pSPS_PPS->m_body = (char *) m_pSPS_PPS + RTMP_PACKET_SIZE;
    int i = 0;
    m_pSPS_PPS->m_body[i++] = 0x17;
    m_pSPS_PPS->m_body[i++] = 0x00;

    m_pSPS_PPS->m_body[i++] = 0x00;
    m_pSPS_PPS->m_body[i++] = 0x00;
    m_pSPS_PPS->m_body[i++] = 0x00;

    m_pSPS_PPS->m_body[i++] = 0x01;
    m_pSPS_PPS->m_body[i++] = sps_nalu->data[1];
    m_pSPS_PPS->m_body[i++] = sps_nalu->data[2];
    m_pSPS_PPS->m_body[i++] = sps_nalu->data[3];
    m_pSPS_PPS->m_body[i++] = 0xff;
    /*sps*/
    m_pSPS_PPS->m_body[i++] = 0xe1;
    m_pSPS_PPS->m_body[i++] = (sps_nalu->size >> 8) & 0xff;
    m_pSPS_PPS->m_body[i++] = sps_nalu->size & 0xff;
    memcpy(&m_pSPS_PPS->m_body[i], sps_nalu->data, sps_nalu->size);
    i += sps_nalu->size;
    /*pps*/
    m_pSPS_PPS->m_body[i++] = 0x01;
    m_pSPS_PPS->m_body[i++] = (pps_nalu->size >> 8) & 0xff;
    m_pSPS_PPS->m_body[i++] = pps_nalu->size & 0xff;
    memcpy(&m_pSPS_PPS->m_body[i], pps_nalu->data, pps_nalu->size);
    i += pps_nalu->size;

    m_pSPS_PPS->m_packetType = RTMP_PACKET_TYPE_FLASH_VIDEO;
    m_pSPS_PPS->m_nBodySize = i;
    m_pSPS_PPS->m_nChannel = 0x04;
    m_pSPS_PPS->m_nTimeStamp = 0;
    m_pSPS_PPS->m_hasAbsTimestamp = 0;
    m_pSPS_PPS->m_nInfoField2 = m_pRtmp->m_stream_id;
//    free(sps_nalu);
//    free(pps_nalu);
}