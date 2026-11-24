#pragma once
// SNODE powered-bus master (protocol v1, docs/10-bus-protocol.md).
// One instance per row bus, each on its own UART (9600 8N1, half duplex).
// Non-blocking: start_read() / start_discover() run a state machine in loop(); poll done().
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace snode_bus {

static const int16_t RAW_MISSING = 0x7FFF;

struct ProbeResult {
  bool ok{false};
  uint16_t raw{0x7FFF};
  uint16_t spread{0};
  int16_t chip_c100{INT16_MIN};
  uint8_t timeouts{0};
  uint8_t status{0};
};

class SnodeBus : public Component, public uart::UARTDevice {
 public:
  void add_slot(uint32_t serial) { this->slots_.push_back(serial); }
  void set_reply_timeout(uint32_t ms) { this->reply_timeout_ms_ = ms; }
  void set_retries(uint8_t n) { this->retries_ = n; }
  float get_setup_priority() const override { return setup_priority::DATA; }
  void dump_config() override;
  void loop() override;

  // Read every configured slot once. Results via result(i) / raw(i) after done().
  void start_read();
  // Find all probe serials on the bus (binary search, install mode). Results via found().
  void start_discover();
  bool done() const { return this->state_ == State::IDLE; }

  size_t slot_count() const { return this->slots_.size(); }
  const ProbeResult &result(size_t i) const { return this->results_[i]; }
  // Raw count for the report packet: 0x7FFF if the slot is empty, failed or out of range.
  int16_t raw(size_t i) const {
    if (i >= this->results_.size() || !this->results_[i].ok) return RAW_MISSING;
    return (int16_t) this->results_[i].raw;
  }
  const std::vector<uint32_t> &found() const { return this->found_; }

 protected:
  enum class State { IDLE, READ_SEND, READ_WAIT, DISC_SEND, DISC_WAIT };
  static uint8_t crc8_(const uint8_t *d, size_t n);
  void send_frame_(uint8_t cmd, uint32_t addr, const uint8_t *arg, size_t arg_len);
  void drain_rx_();
  bool parse_read_reply_(uint32_t addr, ProbeResult &out);
  void next_read_();
  void next_discover_();

  std::vector<uint32_t> slots_;
  std::vector<ProbeResult> results_;
  std::vector<uint32_t> found_;
  uint32_t reply_timeout_ms_{25};
  uint8_t retries_{2};

  State state_{State::IDLE};
  size_t idx_{0};
  uint8_t tries_{0};
  uint32_t t0_{0};
  std::vector<uint8_t> rx_;

  // Discovery: depth-first search over serial bits, LSB first. Entry = (prefix, bit count).
  struct Node { uint32_t prefix; uint8_t bits; };
  std::vector<Node> stack_;
  Node cur_{0, 0};
};

}  // namespace snode_bus
}  // namespace esphome
