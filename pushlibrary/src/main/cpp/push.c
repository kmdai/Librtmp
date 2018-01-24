//
// Created by kmdai on 18-1-24.
//

#include "librtmp/rtmp.h"

/**
 * 设置url
 * @param r
 * @param url
 */
void set_url(RTMP *r, char *url) {
    RTMP_SetupURL(r, url);
}

/**
 * 初始化rtmp
 * @return
 */
RTMP *rtmp_init() {
    RTMP *rtmp = RTMP_Alloc();
    RTMPPacket
    return rtmp;
}