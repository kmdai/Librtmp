//
// Created by kmdai on 18-5-23.
//

#include "push_flvenc.h"
#include "push_rtmp.h"
#include <string.h>
#include <memory.h>
#include <malloc.h>
// AMF0 marker
#define RTMP_AMF0_Number                     0x00
#define RTMP_AMF0_Boolean                     0x01
#define RTMP_AMF0_String                     0x02
#define RTMP_AMF0_Object                     0x03
#define RTMP_AMF0_MovieClip                 0x04 // reserved, not supported
#define RTMP_AMF0_Null                         0x05
#define RTMP_AMF0_Undefined                 0x06
#define RTMP_AMF0_Reference                 0x07
#define RTMP_AMF0_EcmaArray                 0x08
#define RTMP_AMF0_ObjectEnd                 0x09
#define RTMP_AMF0_StrictArray                 0x0A
#define RTMP_AMF0_Date                         0x0B
#define RTMP_AMF0_LongString                 0x0C
#define RTMP_AMF0_UnSupported                 0x0D
#define RTMP_AMF0_RecordSet                 0x0E // reserved, not supported
#define RTMP_AMF0_XmlDocument                 0x0F
#define RTMP_AMF0_TypedObject                 0x10
// AVM+ object is the AMF3 object.
#define RTMP_AMF0_AVMplusObject             0x11
// origin array whos data takes the same form as LengthValueBytes
#define RTMP_AMF0_OriginStrictArray         0x20

char *put_byte(char *out, uint8_t val) {
    out[0] = val;
    return out + 1;
}

char *put_16byte(char *out, uint16_t val) {
    out[1] = val & 0xFF;
    out[0] = val >> 8;
    return out + 2;
}

char *put_24byte(char *out, uint32_t val) {
    out[2] = (char) (val & 0xff);
    out[1] = (char) (val >> 8);
    out[0] = (char) (val >> 16);
    return out + 3;
}

char *put_32byte(char *out, uint32_t val) {
    out[3] = (char) (val & 0xff);
    out[2] = (char) (val >> 8);
    out[1] = (char) (val >> 16);
    out[0] = (char) (val >> 24);
    return out + 4;
}

char *put_string(char *out, char *string) {
    int len = (int) strlen(string);
    out = put_16byte(out, len);
    memcpy(out, string, len);
    return out + len;
}

char *put_64byte(char *c, double d) {
    unsigned char *ci, *co;
    ci = (unsigned char *) &d;
    co = (unsigned char *) c;
    co[0] = ci[7];
    co[1] = ci[6];
    co[2] = ci[5];
    co[3] = ci[4];
    co[4] = ci[3];
    co[5] = ci[2];
    co[6] = ci[1];
    co[7] = ci[0];
    return c + 8;
}

int find_sps_pps_pos(char *data, int size, int offset, int **prefix) {
    int pos = offset;
    while (pos < size) {
        if (data[pos++] == 0x00 && data[pos++] == 0x00) {
            if (data[pos++] == 0x01) {
                **prefix = 3;
                return pos;
            } else {
                //计数回退
                pos--;
                if (data[pos++] == 0x00 && data[pos++] == 0x01) {
                    **prefix = 4;
                    return pos;
                }
            }
        }
    }
    return pos;
}

int create_AVCVideoData(char **data, char *sps, char *pps, uint32_t spsLen, uint32_t ppsLen) {
    *data = (char *) malloc(spsLen + ppsLen + 16);
    char *body = *data;
    int i = 0;
    body[i++] = 0x17;
    body[i++] = 0x00;

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    body[i++] = 0x01;
    body[i++] = sps[1];
    body[i++] = sps[2];
    body[i++] = sps[3];
    body[i++] = (char) 0xff;

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

    return i;
}

int create_MetaData(char **data, double framerate, double videodatarate, double videocodecid,
                    double width,
                    double height, double audiocodecid, double audiodatarate,
                    double audiosamplerate, double audiosamplesize, int stereo) {
    SRS_LOGE("---create_MetaData");
    char *out = (char *) malloc(512);
    char *start = out;
//    char *out = *data;
    out = put_16byte(out, RTMP_AMF0_String);
    out = put_string(out, "onMetaData");
    out = put_16byte(out, 8);
    out = put_32byte(out, 10);
    //编码方式
    out = put_string(out, "videocodecid");
    out = put_64byte(out, videocodecid);
    out = put_string(out, "framerate");
    out = put_64byte(out, framerate);
    out = put_string(out, "videodatarate");
    out = put_64byte(out, videodatarate);
    out = put_string(out, "width");
    out = put_64byte(out, width);
    out = put_string(out, "height");
    out = put_64byte(out, height);
    out = put_string(out, "audiocodecid");
    out = put_64byte(out, audiocodecid);
    out = put_string(out, "audiodatarate");
    out = put_64byte(out, audiodatarate);
    out = put_string(out, "audiosamplerate");
    out = put_64byte(out, audiosamplerate);
    out = put_string(out, "audiosamplesize");
    out = put_64byte(out, audiosamplesize);
    out = put_string(out, "stereo");
    out = put_byte(out, stereo);
    out = put_24byte(out, RTMP_AMF0_ObjectEnd);
    int size = (int) (out - start);
    *data = (char *) malloc(size);
    memcpy(*data, start, size);
    free(start);
    return size;
}

int create_AVCVideoPacket(char **data, char *sps_pps, int size) {
    int spsS;
    int ppsS;
    int *prefix;
    spsS = find_sps_pps_pos(sps_pps, size, 0, &prefix);
    ppsS = find_sps_pps_pos(sps_pps, size, spsS, &prefix);
    int spsLen = ppsS - spsS - *prefix;
    int ppsLen = size - ppsS;
    char *sps = (char *) malloc(spsLen);
    SRS_LOGE("---prefix=%d,spsLen=%d,ppsLen=%d", *prefix, spsLen, ppsLen);
    memcpy(sps, &sps_pps[*prefix], spsLen);
    char *pps = (char *) malloc(ppsLen);
    memcpy(pps, &sps_pps[ppsS], ppsLen);
    int AVCSize = create_AVCVideoData(data, sps, pps, spsLen, ppsLen);
    free(sps);
    free(pps);
    return AVCSize;
}

int create_VideoPacket(char **data, char *nalu, int type, int size, int time) {
    SRS_LOGE("---create_VideoPacket");
    *data = (char *) malloc(size - 4 + 9);
    char *body = *data;
    int i = 0;
    //默认是非关键帧(2:Pframe  7:AVC)
    body[i++] = 0x27;
    body[i++] = 0x01;

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    if (type == 1) {
        body[0] = 0x17;
    }
    body[i++] = (size >> 24) & 0xff;
    body[i++] = (size >> 16) & 0xff;
    body[i++] = (size >> 8) & 0xff;
    body[i++] = size & 0xff;

    if (type == 1) {
        //关键帧1:Iframe  7:AVC
        body[0] = 0x17;
    }
    memcpy(&body[i], &nalu[4], size);
    i += size;
    return i;
}