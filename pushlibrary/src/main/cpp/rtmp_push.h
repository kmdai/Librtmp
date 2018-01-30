//
// Created by kmdai on 18-1-30.
//

#ifndef LIBRTMP_RTMP_PUSH_H
#define LIBRTMP_RTMP_PUSH_H

#include "librtmp/rtmp.h"
#include <memory.h>
#include <stdio.h>
#include "rtmp_util.h"
/**
 * sps、pps起始头大小（00 00 00 01）
 */
#define SPS_PPS_DIVIDE 4
/**
 * RTMPPacket 包大小
 */
#define RTMP_PACKET_SIZE sizeof(RTMPPacket) + RTMP_MAX_HEADER_SIZE
/**
 * _RTMPMetadata
 * 内部结构体。该结构体主要用于存储和传递元数据信息
 */
typedef struct _RTMPMetadata {
    // video, must be h264 type
    unsigned int nWidth;
    unsigned int nHeight;
    unsigned int nFrameRate;
    unsigned int nSpsLen;
    __uint8_t *Sps;
    unsigned int nPpsLen;
    __uint8_t *Pps;
} RTMPMetadata;

/**
 * _NaluUnit
 * 内部结构体。该结构体主要用于存储和传递Nal单元的类型、大小和数据
 */
typedef struct _NaluUnit {
    int type;
    int size;
    unsigned char *data;
} NaluUnit;
/**
 * nalu头
 */
unsigned int nalu_head_pos;
RTMP *m_pRtmp;
/**
 * sps、pps数据
 */
RTMPPacket m_pSPS_PPS;

/**
 * 设置url
 * @param r
 * @param url
 */
int set_Connect(RTMP *r, char *url);

/**
 * 初始化rtmp
 * @return
 */
void rtmp_init();

void rtmp_free();

/**
 * 读取sps数据
 * @param nalu_sps
 * @param data
 * @return
 */
int read_nalu_sps_pps(NaluUnit *nalu_sps, NaluUnit *nalu_pps, __uint8_t *data, __int32_t size,
                      __int32_t offset);

/**
 * 发送sps、pps数据
 * @param data
 * @param sps_nalu
 * @param pps_nalu
 * @return
 */
int send_sps_pps(RTMPMetadata *data, NaluUnit *sps_nalu, NaluUnit *pps_nalu);

/**
 * 发送rtmp包，
 * @param packet
 * @param add_queue 是否添加发送队列
 * @return
 */
int send_rtmp_packet(NaluUnit *naluUnit, int keyFrame, int timeStamp, int add_queue);

#endif //LIBRTMP_RTMP_PUSH_H
