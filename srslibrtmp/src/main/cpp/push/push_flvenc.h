//
// Created by kmdai on 18-5-23.
//

#ifndef LIBRTMP_PUSH_FLVENC_H
#define LIBRTMP_PUSH_FLVENC_H

#include <stdio.h>
#include <stdbool.h>

/**
 *
 * @param framerate 帧率
 * @param videodatarate 比特率
 * @param videocodecid 视频编码方式
 * @param width 宽
 * @param height 高
 * @param audiocodecid 音频编码方式
 * @param audiodatarate 音频码流
 * @param audiosamplerate 音频采样率
 * @param audiosamplesize  采样大小
 * @param stereo 立体声
 * @return mateDateSize
 */
int
create_MetaData(char **data, double framerate, double videodatarate, double videocodecid,
                double width,
                double height,
                double audiocodecid, double audiodatarate, double audiosamplerate,
                double audiosamplesize, int stereo);

int create_AVCVideoPacket(char **data,char* sps_pps,int size);

///**
// *
// * @param sps
// * @param pps
// * @param size
// * @return
// */
//int create_AVCVideoPacket(char **body, char *sps, char *pps, uint32_t spsLen, uint32_t ppsLen);

#endif //LIBRTMP_PUSH_FLVENC_H
