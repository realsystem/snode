// Host tests for the portable probe code: gcc -I../src ../src/*.c test_host.c && ./a.out
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "measure.h"
#include "crc8.h"
#include "probe_id.h"
#include "hal.h"

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL: " __VA_ARGS__); printf("\n"); } } while (0)

// Fake hardware: charge time = base + noise, with optional timeouts.
static uint16_t fake_base = 800;
static int fake_timeout_every = 0, fake_calls = 0;
void hal_electrode_discharge(void) {}
uint16_t hal_electrode_charge_and_time(void) {
  fake_calls++;
  if (fake_timeout_every && fake_calls % fake_timeout_every == 0) return 0xFFFF;
  return (uint16_t) (fake_base + (rand() % 21) - 10);
}
int16_t hal_chip_temp_c100(void) { return 1234; }
uint32_t hal_probe_serial(void) { return 0x00012345; }

int main(void) {
  // CRC-8: Maxim example ROM 0x A2 00 00 00 01 B8 1C 02 has CRC 0xA2 over the first 7 bytes (AN27).
  const uint8_t rom_ex[7] = {0x02, 0x1C, 0xB8, 0x01, 0x00, 0x00, 0x00};
  CHECK(crc8_maxim(rom_ex, 7) == 0xA2, "crc8 AN27 example: got 0x%02X", crc8_maxim(rom_ex, 7));
  // CRC over data + its CRC must be 0.
  uint8_t rom[8];
  probe_rom_code(hal_probe_serial(), rom);
  CHECK(rom[0] == 0x28, "family code");
  CHECK(crc8_maxim(rom, 8) == 0, "rom crc self-check");
  uint8_t sp[9];
  probe_scratchpad(0x0550, 2150, sp);
  CHECK(sp[0] == 0x51 && sp[1] == 0x05, "85 C power-on value avoided");
  CHECK((int8_t) sp[2] == 21, "chip temp in TH");
  CHECK(crc8_maxim(sp, 9) == 0, "scratchpad crc self-check");
  // Trimmed mean.
  uint16_t s[10] = {5, 100, 101, 99, 100, 100, 102, 98, 100, 9000};
  uint16_t spread;
  uint16_t m = measure_trimmed_mean(s, 10, 1, &spread);
  CHECK(m == 100, "trimmed mean: got %u", m);
  CHECK(spread == 4, "spread: got %u", spread);
  // Full measurement with noise.
  measurement_t r;
  srand(1);
  measure_run(&r);
  CHECK(r.raw >= 795 && r.raw <= 805, "measure_run raw: got %u", r.raw);
  CHECK(r.timeouts == 0 && r.chip_c100 == 1234, "measure_run fields");
  // Some timeouts: still valid.
  fake_timeout_every = 5; fake_calls = 0;
  measure_run(&r);
  CHECK(r.raw != 0x7FFF && r.timeouts == 12, "partial timeouts: raw %u timeouts %u", r.raw, r.timeouts);
  // Mostly timeouts: failed.
  fake_timeout_every = 1; fake_calls = 0;
  measure_run(&r);
  CHECK(r.raw == 0x7FFF, "all timeouts -> failed");
  printf(fails ? "%d test(s) failed\n" : "all host tests passed\n", fails);
  return fails ? 1 : 0;
}
