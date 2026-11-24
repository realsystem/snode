#pragma once
// RV-3028-C7 RTC for SNODE (design log D29, D33).
// The node is unpowered between reports; this RTC's alarm (INT) switches its power on.
// Register addresses and bits follow the RV-3028-C7 application manual (UNVERIFIED until bench test T2).
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace rv3028 {

class RV3028Component : public time::RealTimeClock, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // State captured at boot, before anything is changed.
  bool alarm_fired_at_boot() const { return this->alarm_at_boot_; }
  bool power_on_reset_at_boot() const { return this->por_at_boot_; }
  // False after a power-on reset until a valid time has been written.
  bool time_valid() const { return this->time_valid_; }

  uint32_t read_epoch();                 // UNIX time counter (UTC), 0 on error
  bool write_epoch(uint32_t epoch);      // sets UNIX counter + calendar, clears PORF
  bool set_alarm_epoch(uint32_t epoch);  // date/hour/minute alarm (UTC), clears AF, enables INT
  bool clear_alarm_flag();               // releases INT (cuts node power if HOLD is off)

 protected:
  bool update_bits_(uint8_t reg, uint8_t clear_mask, uint8_t set_mask);
  bool alarm_at_boot_{false};
  bool por_at_boot_{false};
  bool time_valid_{false};
};

}  // namespace rv3028
}  // namespace esphome
