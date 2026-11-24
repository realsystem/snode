#include "crc8.h"
uint8_t crc8_maxim(const uint8_t *data, size_t len) {
  uint8_t crc = 0;
  while (len--) {
    uint8_t b = *data++;
    for (int i = 0; i < 8; i++) {
      uint8_t mix = (crc ^ b) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      b >>= 1;
    }
  }
  return crc;
}
