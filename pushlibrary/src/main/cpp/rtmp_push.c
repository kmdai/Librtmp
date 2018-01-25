//
// Created by kmdai on 18-1-24.
//

#include "librtmp/rtmp.h"
#include <memory.h>

RTMP *m_pRtmp;

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