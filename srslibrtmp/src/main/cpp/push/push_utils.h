//
// Created by 哔哩哔哩 on 2019/3/13.
//

#ifndef LIBRTMP_PUSH_UTILS_H
#define LIBRTMP_PUSH_UTILS_H
//#define BITSTREAM_WRITER_LE

#include <android/log.h>

#ifdef __cplusplus
extern "C"
{
#endif
#define SRS_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "push", __VA_ARGS__))

#define AV_WB32(p, val) do {                 \
        uint32_t d = (val);                  \
        ((uint8_t*)(p))[3] = (d);            \
        ((uint8_t*)(p))[2] = (d)>>8;         \
        ((uint8_t*)(p))[1] = (d)>>16;        \
        ((uint8_t*)(p))[0] = (d)>>24;        \
    } while(0)


typedef struct PutBitContext {
    uint32_t bit_buf;
    int bit_left;
    uint8_t *buf, *buf_ptr, *buf_end;
    int size_in_bits;
} PutBitContext;

/**
 * Initialize the PutBitContext s.
 *
 * @param buffer the buffer where to put bits
 * @param buffer_size the size in bytes of buffer
 */
static inline void init_put_bits(PutBitContext *s, uint8_t *buffer,
                                 int buffer_size) {
    if (buffer_size < 0) {
        buffer_size = 0;
        buffer = NULL;
    }

    s->size_in_bits = 8 * buffer_size;
    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;
    s->buf_ptr = s->buf;
    s->bit_left = 32;
    s->bit_buf = 0;
}

/**
 * Write up to 31 bits into a bitstream.
 * Use put_bits32 to write 32 bits.
 */
static inline void put_bits(PutBitContext *s, int n, unsigned int value) {
    unsigned int bit_buf;
    int bit_left;
    bit_buf = s->bit_buf;
    bit_left = s->bit_left;
    if (n < bit_left) {
        bit_buf = (bit_buf << n) | value;
        bit_left -= n;
    } else {
        bit_buf <<= bit_left;
        bit_buf |= value >> (n - bit_left);
        if (3 < s->buf_end - s->buf_ptr) {
            AV_WB32(s->buf_ptr, bit_buf);
            s->buf_ptr += 4;
        }
        bit_left += 32 - n;
        bit_buf = value;
    }
    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
    s->size_in_bits -= n;
}


/**
 * Pad the end of the output stream with zeros.
 */
static inline void flush_put_bits(PutBitContext *s) {
    if (s->bit_left < 32) {
        s->bit_buf <<= s->bit_left;
    }
    while (s->bit_left < 32) {
        *s->buf_ptr++ = s->bit_buf >> 24;
        s->bit_buf <<= 8;
        s->bit_left += 8;
    }
    s->bit_left = 32;
    s->bit_buf = 0;
}

static inline void addADTStoPacket(char *packet, int packetLen) {
    int profile = 2; // AAC LC
    int freqIdx = 4; // 44.1KHz
    int chanCfg = 1; // CPE

    // fill in ADTS data
    packet[0] = (char) 0xFF;
    packet[1] = (char) 0xF9;
    packet[2] = (char) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
    packet[3] = (char) (((chanCfg & 3) << 6) + (packetLen >> 11));
    packet[4] = (char) ((packetLen & 0x7FF) >> 3);
    packet[5] = (char) (((packetLen & 7) << 5) + 0x1F);
    packet[6] = (char) 0xFC;
}
#ifdef __cplusplus
};
#endif
#endif

