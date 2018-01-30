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
int set_Connect(RTMP *r, char *url) {
    if (RTMP_SetupURL(r, url)) {
        RTMP_EnableWrite(r);
        //尝试链接
        if (!RTMP_Connect(r, NULL)) {
        }
    }
    return -1;
}


/**
 * 初始化rtmp
 * @return
 */
void rtmp_init() {
    m_pRtmp = RTMP_Alloc();
}

void rtmp_free() {
    //关闭rtmp
    RTMP_Close(m_pRtmp);
    //释放
    RTMP_Free(m_pRtmp);
}

/**
 * 读取sps|pps数据
 * @param nalu_sps
 * @param data
 * @return
 */
int read_nalu_sps_pps(NaluUnit *nalu_sps, NaluUnit *nalu_pps, __uint8_t *data, __int32_t size,
                      __int32_t offset) {
    for (int i = 0; i < size; i++) {
        LOG_V("data-----:", "%x", data[i]);
    }
    __int32_t nalu_pos = 0 + offset;
    //sps数据起始位
    __int32_t sps_pos = find_sps_pps_pos(data, size, offset);
    LOG_V("read_nalu_sps_pps----", "%d", sps_pos);
    __int32_t pps_pos = find_sps_pps_pos(data, size, sps_pos);
    LOG_V("read_nalu_pps----", "%d", pps_pos);

    nalu_sps = (NaluUnit *) malloc(sizeof(NaluUnit));
    nalu_sps->size = pps_pos - sps_pos - SPS_PPS_DIVIDE;
    nalu_sps->data = (__uint8_t *) malloc(nalu_sps->size);
    memcpy(nalu_sps->data, data + sps_pos, nalu_sps->size);
    nalu_sps->type = nalu_sps->data[0] & 0x1f;

    nalu_pps = (NaluUnit *) malloc(sizeof(NaluUnit));
    nalu_pps->size = size - pps_pos;
    nalu_pps->data = (__uint8_t *) malloc(nalu_pps->size);
    memcpy(nalu_pps->data, data + pps_pos, (size_t) nalu_pps->size);
    for (int i = 0; i < nalu_pps->size; i++) {
        LOG_V("pps->data-----:", "%x", nalu_pps->data[i]);
    }
    return 1;
}

int send_sps_pps(RTMPMetadata *data, NaluUnit *sps_nalu, NaluUnit *pps_nalu) {
    RTMPPacket *packet;
    packet = (RTMPPacket *) malloc(RTMP_PACKET_SIZE + sps_nalu->size + pps_nalu->size);
    packet->m_body = (char *) packet + RTMP_PACKET_SIZE;
    int i = 0;
    packet->m_body[i++] = 0x17;
    packet->m_body[i++] = 0x00;

    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    packet->m_body[i++] = 0x01;
    packet->m_body[i++] = sps_nalu->data[1];
    packet->m_body[i++] = sps_nalu->data[2];
    packet->m_body[i++] = sps_nalu->data[3];
    packet->m_body[i++] = 0xff;
    /*sps*/
    packet->m_body[i++] = 0xe1;
    packet->m_body[i++] = (sps_nalu->size >> 8) & 0xff;
    packet->m_body[i++] = sps_nalu->size & 0xff;
    memcpy(&packet->m_body[i], sps_nalu->data, sps_nalu->size);
    i += sps_nalu->size;
    /*pps*/
    packet->m_body[i++] = 0x01;
    packet->m_body[i++] = (pps_nalu->size >> 8) & 0xff;
    packet->m_body[i++] = pps_nalu->size & 0xff;
    memcpy(&packet->m_body[i], pps_nalu->data, pps_nalu->size);
    i += pps_nalu->size;

    packet->m_packetType = RTMP_PACKET_TYPE_FLASH_VIDEO;
    packet->m_nBodySize = i;
    packet->m_nChannel = 0x04;
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_nInfoField2 = m_pRtmp->m_stream_id;

    int ret = RTMP_SendPacket(m_pRtmp, packet, 1);
    free(packet);
    return ret;
}

int send_rtmp_packet(NaluUnit *naluUnit, int keyFrame, int timeStamp, int add_queue) {
    __uint8_t *body = (__uint8_t *) malloc(naluUnit->size + 9);
    int i = 1;
    //默认是非关键帧(2:Pframe  7:AVC)
    body[i++] = 0x27;
    body[i++] = 0x01;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    body[i++] = naluUnit->size >> 24 & 0xff;
    body[i++] = naluUnit->size >> 16 & 0xff;
    body[i++] = naluUnit->size >> 8 & 0xff;
    body[i++] = naluUnit->size & 0xff;

    memcpy(&body[i], naluUnit->data, naluUnit->size);
    if (keyFrame) {
        //1:Iframe  7:AVC
        body[0] = 0x17;
    }
    RTMPPacket *packet;
}