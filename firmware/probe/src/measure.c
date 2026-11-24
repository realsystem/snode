// Portable measurement logic for the SNODE probe (docs/09-probe-design.md, section 4).
#include "measure.h"
#include "hal.h"

static void sort_u16(uint16_t *a, int n) {   // insertion sort: n = 64, runs once per wake
  for (int i = 1; i < n; i++) {
    uint16_t v = a[i];
    int j = i - 1;
    while (j >= 0 && a[j] > v) { a[j + 1] = a[j]; j--; }
    a[j + 1] = v;
  }
}

uint16_t measure_trimmed_mean(uint16_t *s, int n, int trim, uint16_t *spread) {
  if (n <= 2 * trim) return 0x7FFF;
  sort_u16(s, n);
  uint32_t sum = 0;
  for (int i = trim; i < n - trim; i++) sum += s[i];
  if (spread) *spread = s[n - trim - 1] - s[trim];
  return (uint16_t) ((sum + (uint32_t) (n - 2 * trim) / 2) / (uint32_t) (n - 2 * trim));
}

void measure_run(measurement_t *out) {
  uint16_t s[MEASURE_SAMPLES];
  int n = 0;
  uint8_t to = 0;
  for (int i = 0; i < MEASURE_SAMPLES; i++) {
    hal_electrode_discharge();
    uint16_t t = hal_electrode_charge_and_time();
    if (t == 0xFFFF) { to++; continue; }
    s[n++] = t;
  }
  out->timeouts = to;
  if (n < MEASURE_SAMPLES / 2) {          // more than half timed out: open or shorted electrode
    out->raw = 0x7FFF;
    out->spread = 0;
  } else {
    uint16_t trim = (uint16_t) ((n * MEASURE_TRIM) / MEASURE_SAMPLES);
    out->raw = measure_trimmed_mean(s, n, trim, &out->spread);
    if (out->raw >= 0x7FFF) out->raw = 0x7FFE;   // keep 0x7FFF for "failed"
  }
  out->chip_c100 = hal_chip_temp_c100();
}
