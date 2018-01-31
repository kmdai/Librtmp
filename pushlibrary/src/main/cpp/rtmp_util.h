//
// Created by kmdai on 18-1-29.
//

#ifndef LIBRTMP_RTMP_UTIL_H
#define LIBRTMP_RTMP_UTIL_H

#include <stdint.h>
#include "android/log.h"

#define LOG_V(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)


/**
 * 查找sps、pps起始位置(00 00 01|00 00 00 01)
 * @param data 数据源
 * @param size 大小
 * @param offset 偏移量
 * @return
 */
__int32_t find_sps_pps_pos(__uint8_t *data, __int32_t size, __int32_t offset);

#endif //LIBRTMP_RTMP_UTIL_H
