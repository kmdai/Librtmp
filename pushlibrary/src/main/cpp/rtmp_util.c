//
// Created by kmdai on 18-1-29.
//

#include "rtmp_util.h"


__int32_t find_sps_pps_pos(__uint8_t *data, __int32_t size, __int32_t offset) {
    __int32_t pos = offset;
    while (pos < size) {
        if (data[pos++] == 0x00 && data[pos++] == 0x00) {
            if (data[pos++] == 0x01) {
                return pos;
            } else {
                //计数回退
                pos--;
                if (data[pos++] == 0x00 && data[pos++] == 0x01) {
                    return pos;
                }
            }
        }
    }
    return pos;
}