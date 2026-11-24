// SNODE probe: PY32F002A hardware layer — TO BE COMPLETED against the py32f0-template LL drivers.
// Not compiled yet. Pin numbers are placeholders until the ESSOP10 pinout is checked in the datasheet.
//
// Needed:
//  - Clock: HSI 8 MHz at power-up for measurement timing (24 MHz optional for finer ticks).
//  - CHG pin: push-pull output; drives the electrode through R_CHG (1 MΩ).
//  - COMP1: + input = electrode node, − input = REF divider (R_REF1 = R_REF2 = 100 kΩ, ratiometric 0.5 VDD).
//    Output: interrupt or polled flag; TIM1 free-running at HCLK captures the tick count.
//  - Discharge: CHG low for >= 5 x R_CHG x C_max (C_max ~ 200 pF -> 1 ms).
//  - ADC: internal temperature sensor channel, factory calibration values if the part has them (check datasheet).
//  - Serial: 32-bit value at a fixed flash address written by tools at programming time.
//  - Bus side (protocol layer, after the bus choice — design log D35):
//      LINE pin (EXTI / COMP2) and SINK pin (gate of the pull-down / current-sink MOSFET).
#include "hal.h"

#define PROBE_SERIAL_ADDR 0x08004FFCu   // last word of 20 KB flash (placeholder; reserve it in the linker script)

void hal_electrode_discharge(void) {
  // TODO: CHG output low; busy-wait ~1 ms.
}

uint16_t hal_electrode_charge_and_time(void) {
  // TODO: TIM1->CNT = 0; CHG high; wait for COMP1 output high or CNT > timeout; return CNT; CHG low.
  return 0xFFFF;
}

int16_t hal_chip_temp_c100(void) {
  // TODO: ADC temperature channel with calibration.
  return INT16_MIN;
}

uint32_t hal_probe_serial(void) { return *(const volatile uint32_t *) PROBE_SERIAL_ADDR; }
