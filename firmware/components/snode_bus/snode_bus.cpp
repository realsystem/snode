#include "snode_bus.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace snode_bus {

static const char *const TAG = "snode_bus";

// Protocol v1 (docs/10-bus-protocol.md)
static const uint8_t SYNC_NODE = 0xA5;
static const uint8_t SYNC_PROBE = 0x5A;
static const uint8_t CMD_READ = 0x01;
static const uint8_t CMD_DISCOVER = 0x02;
static const uint8_t RSP_READ = 0x81;
static const size_t READ_REPLY_LEN = 15;  // 5A 81 addr(4) raw(2) spread(2) chip(2) timeouts status crc
static const uint32_t GUARD_MS = 1;       // ignore RX this long after our own frame (edge currents)

uint8_t SnodeBus::crc8_(const uint8_t *d, size_t n) {  // Dallas/Maxim CRC-8, same as the probe
  uint8_t crc = 0;
  while (n--) {
    uint8_t b = *d++;
    for (int i = 0; i < 8; i++) {
      uint8_t mix = (crc ^ b) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      b >>= 1;
    }
  }
  return crc;
}

void SnodeBus::drain_rx_() {
  uint8_t b;
  while (this->available() > 0) this->read_byte(&b);
}

void SnodeBus::send_frame_(uint8_t cmd, uint32_t addr, const uint8_t *arg, size_t arg_len) {
  uint8_t f[12];
  size_t n = 0;
  f[n++] = SYNC_NODE;
  f[n++] = cmd;
  for (int i = 0; i < 4; i++) f[n++] = (uint8_t) ((addr >> (8 * i)) & 0xFF);
  for (size_t i = 0; i < arg_len && n < sizeof(f) - 1; i++) f[n++] = arg[i];
  f[n] = crc8_(f + 1, n - 1);  // CRC over cmd..arg (not the sync byte)
  n++;
  this->drain_rx_();
  this->write_array(f, n);
  this->flush();  // wait until the last stop bit is out
  delay(GUARD_MS);
  this->drain_rx_();
  this->rx_.clear();
  this->t0_ = millis();
}

bool SnodeBus::parse_read_reply_(uint32_t addr, ProbeResult &out) {
  // Find a sync byte, then check length, type, address and CRC.
  size_t s = 0;
  while (s < this->rx_.size() && this->rx_[s] != SYNC_PROBE) s++;
  if (this->rx_.size() - s < READ_REPLY_LEN) return false;
  const uint8_t *p = this->rx_.data() + s;
  if (p[1] != RSP_READ) return false;
  if (crc8_(p + 1, READ_REPLY_LEN - 2) != p[READ_REPLY_LEN - 1]) return false;
  uint32_t a = (uint32_t) p[2] | ((uint32_t) p[3] << 8) | ((uint32_t) p[4] << 16) | ((uint32_t) p[5] << 24);
  if (a != addr) return false;
  out.raw = (uint16_t) (p[6] | (p[7] << 8));
  out.spread = (uint16_t) (p[8] | (p[9] << 8));
  out.chip_c100 = (int16_t) (p[10] | (p[11] << 8));
  out.timeouts = p[12];
  out.status = p[13];
  out.ok = out.raw != 0x7FFF;
  return true;
}

void SnodeBus::start_read() {
  this->results_.assign(this->slots_.size(), ProbeResult{});
  this->idx_ = 0;
  this->tries_ = 0;
  this->state_ = State::READ_SEND;
}

void SnodeBus::next_read_() {
  this->tries_ = 0;
  this->idx_++;
  this->state_ = State::READ_SEND;
}

void SnodeBus::start_discover() {
  this->found_.clear();
  this->stack_.clear();
  this->stack_.push_back(Node{0, 0});
  this->state_ = State::DISC_SEND;
}

void SnodeBus::next_discover_() { this->state_ = this->stack_.empty() ? State::IDLE : State::DISC_SEND; }

void SnodeBus::loop() {
  uint8_t b;
  switch (this->state_) {
    case State::IDLE:
      return;

    case State::READ_SEND: {
      while (this->idx_ < this->slots_.size() && this->slots_[this->idx_] == 0) this->idx_++;  // empty slots
      if (this->idx_ >= this->slots_.size()) {
        this->state_ = State::IDLE;
        return;
      }
      this->send_frame_(CMD_READ, this->slots_[this->idx_], nullptr, 0);
      this->state_ = State::READ_WAIT;
      return;
    }

    case State::READ_WAIT: {
      while (this->available() > 0 && this->read_byte(&b)) this->rx_.push_back(b);
      if (this->parse_read_reply_(this->slots_[this->idx_], this->results_[this->idx_])) {
        this->next_read_();
        return;
      }
      if (millis() - this->t0_ > this->reply_timeout_ms_) {
        if (this->tries_++ < this->retries_) {
          this->state_ = State::READ_SEND;
        } else {
          ESP_LOGW(TAG, "slot %u (serial %08X): no reply", (unsigned) (this->idx_ + 1),
                   (unsigned) this->slots_[this->idx_]);
          this->next_read_();
        }
      }
      return;
    }

    case State::DISC_SEND: {
      // Ask: "is there a probe whose lowest `bits` serial bits equal `prefix`?" Any reply = yes.
      this->cur_ = this->stack_.back();
      this->stack_.pop_back();
      uint8_t arg = this->cur_.bits;
      this->send_frame_(CMD_DISCOVER, this->cur_.prefix, &arg, 1);
      this->state_ = State::DISC_WAIT;
      return;
    }

    case State::DISC_WAIT: {
      bool any = false;
      while (this->available() > 0 && this->read_byte(&b)) any = true;  // collisions are fine: any byte = yes
      if (!any && millis() - this->t0_ <= this->reply_timeout_ms_) return;
      if (any) {
        if (this->cur_.bits >= 32) {
          if (this->found_.size() < 32) this->found_.push_back(this->cur_.prefix);
          ESP_LOGI(TAG, "found probe %08X", (unsigned) this->cur_.prefix);
        } else {
          uint8_t n = this->cur_.bits;
          this->stack_.push_back(Node{this->cur_.prefix | (1u << n), (uint8_t) (n + 1)});
          this->stack_.push_back(Node{this->cur_.prefix, (uint8_t) (n + 1)});
        }
      }
      this->next_discover_();
      return;
    }
  }
}

void SnodeBus::dump_config() {
  ESP_LOGCONFIG(TAG, "SNODE bus (protocol v1):");
  ESP_LOGCONFIG(TAG, "  Slots: %u, reply timeout %u ms, retries %u", (unsigned) this->slots_.size(),
                (unsigned) this->reply_timeout_ms_, (unsigned) this->retries_);
  for (size_t i = 0; i < this->slots_.size(); i++)
    ESP_LOGCONFIG(TAG, "  Slot %u: %08X", (unsigned) (i + 1), (unsigned) this->slots_[i]);
}

}  // namespace snode_bus
}  // namespace esphome
