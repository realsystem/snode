#pragma once
#include <stdint.h>
#include <stddef.h>
// Dallas/Maxim 1-Wire CRC-8 (polynomial x^8 + x^5 + x^4 + 1, reflected 0x8C), used for the
// 1-Wire option (ROM code and scratchpad) and for frame checks in the powered-bus option.
uint8_t crc8_maxim(const uint8_t *data, size_t len);
