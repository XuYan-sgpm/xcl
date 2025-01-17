//
// Created by 徐琰 on 2023/8/22.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/define.h>

// uint8_t
// crc4_itu(uint8_t* entry, uint16_t length);
// uint8_t
// crc5_epc(uint8_t* entry, uint16_t length);
// uint8_t
// crc5_itu(uint8_t* entry, uint16_t length);
// uint8_t
// crc5_usb(uint8_t* entry, uint16_t length);
// uint8_t
// crc6_itu(uint8_t* entry, uint16_t length);
// uint8_t
// crc7_mmc(uint8_t* entry, uint16_t length);
// uint8_t
// crc8(uint8_t* entry, uint16_t length);
// uint8_t
// crc8_itu(uint8_t* entry, uint16_t length);
// uint8_t
// crc8_rohc(uint8_t* entry, uint16_t length);
// uint8_t
// crc8_maxim(uint8_t* entry, uint16_t length); // DS18B20
// uint16_t
// crc16_ibm(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_maxim(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_usb(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_modbus(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_ccitt(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_ccitt_false(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_x25(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_xmodem(uint8_t* entry, uint16_t length);
// uint16_t
// crc16_dnp(uint8_t* entry, uint16_t length);
// uint32_t
// crc32(uint8_t* entry, uint16_t length);
// uint32_t
// crc32_mpeg_2(uint8_t* entry, uint16_t length);
uint32_t crc32Init();
uint32_t crc32Update(uint32_t val, uint8_t* data, uint64_t length);
uint32_t crc32Final(uint32_t val);

#ifdef __cplusplus
}
#endif
