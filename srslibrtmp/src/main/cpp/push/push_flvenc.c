//
// Created by kmdai on 18-5-23.
//

#include "push_flvenc.h"
#include <string.h>
#include <memory.h>
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
    out[1] = (char) (val & 0xFF);
    out[0] = (char) (val >> 8);
    return out + 2;
}

char *put_24byte(char *out, uint32_t val) {
    out[2] = (char) (val & 0xff);
    out[1] = (char) (val >> 8);
    out[0] = (char) (val >> 16);
    return out + 3;
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

int create_MetaData(char *data, double framerate, double videodatarate, double videocodecid,
                    double width,
                    double height, double audiocodecid, double audiodatarate,
                    double audiosamplerate, double audiosamplesize, int stereo) {
    char out = data;
    out = put_16byte(out, RTMP_AMF0_String);
    out = put_string(out, "onMetaData");

    out = put_string(out, "hasVideo");
    return 0;
}
