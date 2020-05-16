#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/fan/fan_state.h"
#include "esphome/components/cc1101/cc1101.h"
#define RCSWITCH_MAX_CHANGES 67

namespace esphome {
namespace kaku_cc1101 {

/**
 * Description of a single pule, which consists of a high signal
 * whose duration is "high" times the base pulse length, followed
 * by a low signal lasting "low" times the base pulse length.
 * Thus, the pulse overall lasts (high+low)*pulseLength
 */
struct HighLow {
  uint8_t high;
  uint8_t low;
};

/**
 * A "protocol" describes how zero and one bits are encoded into high/low
 * pulses.
 */
struct Protocol {
  /** base pulse length in microseconds, e.g. 350 */
  uint16_t pulseLength;

  HighLow syncFactor;
  HighLow zero;
  HighLow one;

  /**
   * If true, interchange high and low logic levels in all transmissions.
   *
   * By default, RCSwitch assumes that any signals it sends or receives
   * can be broken down into pulses which start with a high signal level,
   * followed by a a low signal level. This is e.g. the case for the
   * popular PT 2260 encoder chip, and thus many switches out there.
   *
   * But some devices do it the other way around, and start with a low
   * signal level, followed by a high signal level, e.g. the HT6P20B. To
   * accommodate this, one can set invertedSignal to true, which causes
   * RCSwitch to change how it interprets any HighLow struct FOO: It will
   * then assume transmissions start with a low signal lasting
   * FOO.high*pulseLength microseconds, followed by a high signal lasting
   * FOO.low*pulseLength microseconds.
   */
  bool invertedSignal;
};

class KakuCC1101Component : public esphome::Component, public cc1101::CC1101Device {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_rf_address(uint64_t address) {
    this->rf_address_.resize(3, 0);
    for (uint8_t i = 0; i < 3; i++) {
      this->rf_address_[i] = (address >> (8 * (2 - i))) & 0xff;
    }
  }

  void join();
  void send_command(std::string command);

 private:
  constexpr static Protocol proto[] = {
      {350, {1, 31}, {1, 3}, {3, 1}, false},     // protocol 1
      {650, {1, 10}, {1, 2}, {2, 1}, false},     // protocol 2
      {100, {30, 71}, {4, 11}, {9, 6}, false},   // protocol 3
      {380, {1, 6}, {1, 3}, {3, 1}, false},      // protocol 4
      {500, {6, 14}, {1, 2}, {2, 1}, false},     // protocol 5
      {450, {23, 1}, {1, 2}, {2, 1}, true},      // protocol 6 (HT6P20B)
      {150, {2, 62}, {1, 6}, {6, 1}, false},     // protocol 7 (HS2303-PT, i. e. used in AUKEY Remote)
      {200, {3, 130}, {7, 16}, {3, 16}, false},  // protocol 8 Conrad RS-200 RX
      {200, {130, 7}, {16, 7}, {16, 3}, true},   // protocol 9 Conrad RS-200 TX
      {365, {18, 1}, {3, 1}, {1, 3}, true},      // protocol 10 (1ByOne Doorbell)
      {270, {36, 1}, {1, 2}, {2, 1}, true},      // protocol 11 (HT12E)
      {320, {36, 1}, {1, 2}, {2, 1}, true}       // protocol 12 (SM5212)
  };
  constexpr static uint8_t numProto = sizeof(proto) / sizeof(proto[0]);

  static int nReceiveTolerance;
  volatile static unsigned long nReceivedValue;
  volatile static unsigned int nReceivedBitlength;
  volatile static unsigned int nReceivedDelay;
  volatile static unsigned int nReceivedProtocol;
  const static unsigned int nSeparationLimit;
  /*
   * timings[0] contains sync timing, followed by a number of bits
   */
  static unsigned int timings[RCSWITCH_MAX_CHANGES];

  std::vector<uint8_t> rf_address_;
  bool next_update_{true};
  std::vector<uint8_t> packet_;
  std::uint8_t counter_ = 0;
  std::uint8_t tries_ = 0;

  static inline unsigned int diff(int A, int B) { return abs(A - B); }
  std::string format_addr_(std::vector<uint8_t> addr);
  void schedule_send_packet_();
  void init_kaku();
  uint8_t calc_checksum(std::vector<uint8_t> data);
  uint8_t send_packet();
  void prepare_packet(std::string command);

  static void handleInterrupt();
  static bool receiveProtocol(const int p, unsigned int changeCount);
};

}  // namespace kaku_cc1101
}  // namespace esphome
