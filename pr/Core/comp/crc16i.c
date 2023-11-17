//
// Created by klokov on 18.05.2023.
//

#include "crc16i.h"
#include <stdint.h>
#include <stdbool.h>


#define PACKET_LEN    (7)
#define NUM_OF_CYCLES (100000)

static unsigned char reverse_table[16] =
        {
                0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE,
                0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF
        };

uint8_t reverse_bits(uint8_t byte) {
    // Reverse the top and bottom nibble then swap them.
    return (reverse_table[byte & 0b1111] << 4) | reverse_table[byte >> 4];
}

uint16_t reverse_word(uint16_t word) {
    return ((reverse_bits(word & 0xFF) << 8) | reverse_bits(word >> 8));
}

uint16_t crc16_common(uint8_t *data, uint8_t len, uint16_t poly, uint16_t init,
                      uint16_t doXor, bool refIn, bool refOut) {
    uint8_t y;
    uint16_t crc;

    crc = init;
    while (len--) {
        if (refIn)
            crc = ((uint16_t) reverse_bits(*data++) << 8) ^ crc;
        else
            crc = ((uint16_t) * data++ << 8) ^ crc;
        for (y = 0; y < 8; y++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ poly;
            else
                crc = crc << 1;
        }
    }

    if (refOut)
        crc = reverse_word(crc);
    return (crc ^ doXor);
}

// uint16_t crc16_ccitt(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x1021, 0xFFFF, 0x0000, false, false);
// }

// uint16_t crc16_bypass(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x8005, 0x0000, 0x0000, false, false);
// }

// uint16_t crc16_xmodem(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x1021, 0x0000, 0x0000, false, false);
// }

// uint16_t crc16_teledisk(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0xA097, 0x0000, 0x0000, false, false);
// }

//uint16_t crc16_augccitt(uint8_t *data, uint8_t len) {
//    return crc16_common(data, len, 0x1021, 0x1d0f, 0x0000, false, false);
//}

uint16_t crc16_augccitt_false(uint8_t *data, uint8_t len) {
    return crc16_common(data, len, 0x1021, 0xFFFF, 0x0000, false, false);
}

// uint16_t crc16_cdma2000(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0xc867, 0xffff, 0x0000, false, false);
// }

// uint16_t crc16_dds110(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x8005, 0x800d, 0x0000, false, false);
// }

// uint16_t crc16_dect(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x0589, 0x0000, 0x0000, false, false);
// }

// uint16_t crc16_en13757(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x3d65, 0x0000, 0xffff, false, false);
// }

// uint16_t crc16_t10dif(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x8bb7, 0x0000, 0x0000, false, false);
// }

// uint16_t crc16_cms(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x8005, 0xFFFF, 0x0000, false, false);
// }

// uint16_t crc16_modbus(uint8_t *data, uint8_t len) {
//     return crc16_common(data, len, 0x8005, 0xFFFF, 0x0000, true, true);
// }
