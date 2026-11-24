#include "probe_id.h"
#include "crc8.h"

void probe_rom_code(uint32_t serial, uint8_t rom[8]) {
  rom[0] = 0x28;
  rom[1] = (uint8_t) (serial & 0xFF);
  rom[2] = (uint8_t) ((serial >> 8) & 0xFF);
  rom[3] = (uint8_t) ((serial >> 16) & 0xFF);
  rom[4] = (uint8_t) ((serial >> 24) & 0xFF);
  rom[5] = 0x0E;
  rom[6] = 0x5D;
  rom[7] = crc8_maxim(rom, 7);
}

void probe_scratchpad(uint16_t raw, int16_t chip_c100, uint8_t sp[9]) {
  // 0x0550 (85.0 degrees C) is the DS18B20 power-on value; hosts may treat it as "not converted".
  if (raw == 0x0550) raw = 0x0551;
  sp[0] = (uint8_t) (raw & 0xFF);
  sp[1] = (uint8_t) (raw >> 8);
  int16_t c = (chip_c100 == INT16_MIN) ? -128 : (int16_t) (chip_c100 / 100);
  if (c < -128) c = -128;
  if (c > 127) c = 127;
  sp[2] = (uint8_t) (int8_t) c;   // TH: chip temperature, whole degrees C
  sp[3] = 0x00;                   // TL: reserved
  sp[4] = 0x7F;                   // config: 12-bit
  sp[5] = 0xFF;
  sp[6] = 0x0C;
  sp[7] = 0x10;
  sp[8] = crc8_maxim(sp, 8);
}
