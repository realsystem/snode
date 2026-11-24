#include "rv3028.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rv3028 {

static const char *const TAG = "rv3028";

// Registers (RV-3028-C7 application manual)
static const uint8_t REG_SECONDS = 0x00;    // 0x00..0x06: sec, min, hour, weekday, date, month, year (BCD)
static const uint8_t REG_MIN_ALARM = 0x07;  // 0x07..0x09: minute, hour, weekday/date alarm; bit 7 = AE (0 = enabled)
static const uint8_t REG_STATUS = 0x0E;
static const uint8_t REG_CONTROL1 = 0x0F;
static const uint8_t REG_CONTROL2 = 0x10;
static const uint8_t REG_UNIX0 = 0x1B;      // 0x1B..0x1E: 32-bit UNIX time, LSB first
static const uint8_t REG_EE_BACKUP = 0x37;  // RAM mirror of the EEPROM backup register

static const uint8_t STATUS_PORF = 0x01;
static const uint8_t STATUS_AF = 0x04;
static const uint8_t CTRL1_EERD = 0x08;     // 1 = no automatic EEPROM refresh at midnight
static const uint8_t CTRL1_WADA = 0x20;     // 1 = date alarm (not weekday)
static const uint8_t CTRL2_12_24 = 0x02;    // 0 = 24 h
static const uint8_t CTRL2_AIE = 0x08;      // alarm interrupt enable
static const uint8_t BACKUP_TCE = 0x20;     // trickle charge enable
static const uint8_t BACKUP_FEDE = 0x10;    // fast edge detection: manual says keep set
static const uint8_t BACKUP_BSM_MASK = 0x0C;  // backup switchover mode: 00 = disabled

static uint8_t to_bcd(uint8_t v) { return ((v / 10) << 4) | (v % 10); }

bool RV3028Component::update_bits_(uint8_t reg, uint8_t clear_mask, uint8_t set_mask) {
  uint8_t v;
  if (!this->read_byte(reg, &v))
    return false;
  uint8_t n = (v & ~clear_mask) | set_mask;
  if (n == v)
    return true;
  return this->write_byte(reg, n);
}

void RV3028Component::setup() {
  uint8_t status;
  if (!this->read_byte(REG_STATUS, &status)) {
    ESP_LOGE(TAG, "RV-3028 not found");
    this->mark_failed();
    return;
  }
  this->alarm_at_boot_ = (status & STATUS_AF) != 0;
  this->por_at_boot_ = (status & STATUS_PORF) != 0;
  this->time_valid_ = !this->por_at_boot_;

  // Keep the RAM settings below: stop the midnight EEPROM refresh from overwriting them.
  this->update_bits_(REG_CONTROL1, 0, CTRL1_EERD);
  // No backup supply in this design: switchover off, trickle charge off, FEDE on.
  this->update_bits_(REG_EE_BACKUP, BACKUP_TCE | BACKUP_BSM_MASK, BACKUP_FEDE);
  // 24-hour mode.
  this->update_bits_(REG_CONTROL2, CTRL2_12_24, 0);

  if (this->time_valid_)
    this->update();
}

uint32_t RV3028Component::read_epoch() {
  // The counter can tick between byte reads: read until two reads agree.
  for (int i = 0; i < 3; i++) {
    uint8_t a[4], b[4];
    if (!this->read_bytes(REG_UNIX0, a, 4) || !this->read_bytes(REG_UNIX0, b, 4))
      return 0;
    if (memcmp(a, b, 4) == 0)
      return (uint32_t) a[0] | ((uint32_t) a[1] << 8) | ((uint32_t) a[2] << 16) | ((uint32_t) a[3] << 24);
  }
  return 0;
}

void RV3028Component::update() {
  if (!this->time_valid_)
    return;
  uint32_t e = this->read_epoch();
  if (e != 0)
    this->synchronize_epoch_(e);
}

bool RV3028Component::write_epoch(uint32_t epoch) {
  ESPTime t = ESPTime::from_epoch_utc(epoch);
  uint8_t cal[7] = {to_bcd(t.second), to_bcd(t.minute), to_bcd(t.hour),
                    (uint8_t) (t.day_of_week - 1),  // ESPTime: 1 = Sunday; RTC: 0..6
                    to_bcd(t.day_of_month), to_bcd(t.month), to_bcd((uint8_t) (t.year - 2000))};
  uint8_t ux[4] = {(uint8_t) (epoch & 0xFF), (uint8_t) ((epoch >> 8) & 0xFF), (uint8_t) ((epoch >> 16) & 0xFF),
                   (uint8_t) ((epoch >> 24) & 0xFF)};
  if (!this->write_bytes(REG_SECONDS, cal, 7) || !this->write_bytes(REG_UNIX0, ux, 4)) {
    ESP_LOGW(TAG, "time write failed");
    return false;
  }
  this->update_bits_(REG_STATUS, STATUS_PORF, 0);
  this->time_valid_ = true;
  this->synchronize_epoch_(epoch);
  ESP_LOGI(TAG, "time set to %u", (unsigned) epoch);
  return true;
}

bool RV3028Component::set_alarm_epoch(uint32_t epoch) {
  ESPTime t = ESPTime::from_epoch_utc(epoch);
  // Manual: disable the alarm interrupt and clear AF before changing the alarm.
  if (!this->update_bits_(REG_CONTROL2, CTRL2_AIE, 0))
    return false;
  uint8_t al[3] = {to_bcd(t.minute), to_bcd(t.hour), to_bcd(t.day_of_month)};  // AE bits = 0: all three must match
  if (!this->write_bytes(REG_MIN_ALARM, al, 3))
    return false;
  this->update_bits_(REG_CONTROL1, 0, CTRL1_WADA);
  this->update_bits_(REG_STATUS, STATUS_AF, 0);
  bool ok = this->update_bits_(REG_CONTROL2, 0, CTRL2_AIE);
  ESP_LOGI(TAG, "alarm set for %02u:%02u UTC on day %u (epoch %u)", t.hour, t.minute, t.day_of_month,
           (unsigned) epoch);
  return ok;
}

bool RV3028Component::clear_alarm_flag() { return this->update_bits_(REG_STATUS, STATUS_AF, 0); }

void RV3028Component::dump_config() {
  ESP_LOGCONFIG(TAG, "RV-3028-C7:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Woken by alarm: %s", YESNO(this->alarm_at_boot_));
  ESP_LOGCONFIG(TAG, "  Power-on reset seen: %s", YESNO(this->por_at_boot_));
  ESP_LOGCONFIG(TAG, "  Time valid: %s", YESNO(this->time_valid_));
}

}  // namespace rv3028
}  // namespace esphome
