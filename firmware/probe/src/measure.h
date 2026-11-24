#pragma once
#include <stdint.h>

#define MEASURE_SAMPLES 64      // averaged samples per reading
#define MEASURE_TRIM 8          // samples dropped at each end (outliers) before averaging

// Result of one measurement at power-up (stored until the node reads it).
typedef struct {
  uint16_t raw;        // mean charge time in timer ticks (higher = wetter soil); 0x7FFF = failed
  uint16_t spread;     // max - min of the kept samples (noise indicator)
  int16_t chip_c100;   // chip temperature, degrees C x 100
  uint8_t timeouts;    // samples that timed out
} measurement_t;

void measure_run(measurement_t *out);
// Exposed for tests: trimmed mean of n samples (sorts in place).
uint16_t measure_trimmed_mean(uint16_t *s, int n, int trim, uint16_t *spread);
