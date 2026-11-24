#pragma once
#include <stdint.h>
// 1-Wire option: 64-bit ROM = family 0x28 (DS18B20) | 48-bit serial | CRC-8.
// The 32-bit probe serial fills the low 32 bits of the 48-bit field; the top 16 bits are 0x5D0E
// ("SNODE" marker) so probes can be told apart from real DS18B20s.
void probe_rom_code(uint32_t serial, uint8_t rom[8]);
// DS18B20 scratchpad carrying a raw count (option 1-Wire, design log D22):
// bytes 0-1 = raw count as the "temperature" register, 4 = config 0x7F (12-bit), 8 = CRC.
// Chip temperature goes in TH/TL (bytes 2-3, int16 degrees C x 100 / 100 -> whole degrees in byte 2).
void probe_scratchpad(uint16_t raw, int16_t chip_c100, uint8_t sp[9]);
