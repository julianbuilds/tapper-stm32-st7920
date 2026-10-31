#include <stdint.h>

#define CERVEZA_ANCHO 6
#define CERVEZA_ALTO 10
#define CERVEZA_TOTAL_FRAMES 1

const uint8_t frame_cerveza_0[10] = {
    0x00,
    0x70,
    0xF8,
    0xFC,
    0x94,
    0x94,
    0x9C,
    0xF0,
    0xF0,
    0xF0,
};

const uint8_t* const cerveza_frames[CERVEZA_TOTAL_FRAMES] = {
    frame_cerveza_0
};
