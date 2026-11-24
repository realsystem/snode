// SNODE probe: hardware interface used by the portable code (docs/09-probe-design.md).
// hal_py32.c implements it for the PY32F002A; tests/ implements a fake for host tests.
#pragma once
#include <stdint.h>
#include <stdbool.h>

// Electrode RC measurement (one sample):
//  1. hal_electrode_discharge(): drive CHG low long enough to empty the electrode.
//  2. hal_electrode_charge_and_time(): start the timer, drive CHG high through R_CHG,
//     return timer ticks until the comparator sees V(electrode) > V(ref) (ratiometric: ref = k x VDD).
//     Returns 0xFFFF on timeout.
void hal_electrode_discharge(void);
uint16_t hal_electrode_charge_and_time(void);

// Chip temperature (internal sensor), degrees C x 100. INT16_MIN if not available.
int16_t hal_chip_temp_c100(void);

// 32-bit probe serial: written into flash at programming time (printed on the label).
uint32_t hal_probe_serial(void);
