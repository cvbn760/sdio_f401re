//
// Created by klokov on 18.05.2023.
//

#ifndef AUTOCONNEX_CRC16I_H
#define AUTOCONNEX_CRC16I_H

#include <stdint.h>
#include <stdbool.h>

uint8_t reverse_bits(uint8_t byte);

uint16_t reverse_word(uint16_t word);

uint16_t
crc16_common(uint8_t *data, uint8_t len, uint16_t poly, uint16_t init, uint16_t doXor, bool refIn, bool refOut);

// uint16_t crc16_ccitt(uint8_t *data, uint8_t len);

// uint16_t crc16_bypass(uint8_t *data, uint8_t len);

// uint16_t crc16_xmodem(uint8_t *data, uint8_t len);

// uint16_t crc16_teledisk(uint8_t *data, uint8_t len);

//uint16_t crc16_augccitt(uint8_t *data, uint8_t len);

uint16_t crc16_augccitt_false(uint8_t *data, uint8_t len);

// uint16_t crc16_cdma2000(uint8_t *data, uint8_t len);

// uint16_t crc16_dds110(uint8_t *data, uint8_t len);

// uint16_t crc16_dect(uint8_t *data, uint8_t len);

// uint16_t crc16_en13757(uint8_t *data, uint8_t len);

// uint16_t crc16_t10dif(uint8_t *data, uint8_t len);

// uint16_t crc16_cms(uint8_t *data, uint8_t len);

// uint16_t crc16_modbus(uint8_t *data, uint8_t len);

#endif //AUTOCONNEX_CRC16I_H
