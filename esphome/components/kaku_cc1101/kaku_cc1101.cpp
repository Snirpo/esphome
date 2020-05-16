#include "esphome/core/log.h"
#include "kaku_cc1101.h"
#include "kaku_protocol.h"

namespace esphome {
namespace kaku_cc1101 {

static const char *TAG = "itho_ecofanrft";

std::string KakuCC1101Component::format_addr_(std::vector<uint8_t> addr) {
  std::string s;
  char buf[20];
  for (uint8_t a : addr) {
    sprintf(&buf[0], "%02X:", a);
    s += buf;
  }
  s.pop_back();
  return s;
}

void KakuCC1101Component::dump_config() {
  std::string s;

  // ESP_LOGCONFIG(TAG, "Itho EcoFanRft '%s'", this->get_name().c_str());
  ESP_LOGCONFIG(TAG, "  RF Address: '%s'", this->format_addr_(this->rf_address_).c_str());

#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
  std::vector<uint8_t> config = this->cc1101_->read_burst_register(0x00, 47);
  for (uint8_t i = 0; i < config.size(); i++) {
    ESP_LOGCONFIG(TAG, "Config register [%02X] => [%02X]", i, config[i]);
  }
#endif
}
void KakuCC1101Component::setup() {
  if (!this->cc1101_->is_failed()) {
    this->mark_failed();
    return;
  }

  // Setup module for Itho protocol
  this->init_kaku();
}
void KakuCC1101Component::loop() {}

float KakuCC1101Component::get_setup_priority() const { return setup_priority::DATA; }

void KakuCC1101Component::send_command(std::string command) {
  this->prepare_packet(command);
  this->send_packet();

  this->set_timeout("send_command", 40, [this]() { this->schedule_send_packet_(); });
}

void KakuCC1101Component::schedule_send_packet_() {
  uint8_t tries_left = this->send_packet();

  if (tries_left > 0) {
    this->set_timeout("send_command", 50, [this]() { this->schedule_send_packet_(); });
  }
}

void KakuCC1101Component::init_kaku() {
  for (register_setting r : KAKU_CONFIG) {
    this->cc1101_->write_register(r.address, r.data);
  }
  this->cc1101_->write_burst_register(CC1101_PATABLE, KAKU_PATABLE);

  this->cc1101_->write_command_strobe(CC1101_SCAL);
  delay(1);
}

uint8_t KakuCC1101Component::calc_checksum(std::vector<uint8_t> data) {
  uint8_t checksum = 0;
  for (unsigned char i : data) {
    checksum = (checksum + i) & 0xFF;
  }
  return 0 - checksum;
}

void KakuCC1101Component::prepare_packet(std::string command) {}

uint8_t KakuCC1101Component::send_packet() {
  ESP_LOGV(TAG, "%d tries left for sending packet of %d bytes", this->tries_, this->packet_.size());
  if (this->packet_.size() > 0 && this->tries_-- > 0) {
    this->cc1101_->send_data(this->packet_);
  }

  return this->tries_;
}

bool KakuCC1101Component::receiveProtocol(const int p, unsigned int changeCount) {
  const Protocol &pro = proto[p - 1];

  unsigned long code = 0;
  // Assuming the longer pulse length is the pulse captured in timings[0]
  const unsigned int syncLengthInPulses =
      ((pro.syncFactor.low) > (pro.syncFactor.high)) ? (pro.syncFactor.low) : (pro.syncFactor.high);
  const unsigned int delay = KakuCC1101Component::timings[0] / syncLengthInPulses;
  const unsigned int delayTolerance = delay * KakuCC1101Component::nReceiveTolerance / 100;

  /* For protocols that start low, the sync period looks like
   *               _________
   * _____________|         |XXXXXXXXXXXX|
   *
   * |--1st dur--|-2nd dur-|-Start data-|
   *
   * The 3rd saved duration starts the data.
   *
   * For protocols that start high, the sync period looks like
   *
   *  ______________
   * |              |____________|XXXXXXXXXXXXX|
   *
   * |-filtered out-|--1st dur--|--Start data--|
   *
   * The 2nd saved duration starts the data
   */
  const unsigned int firstDataTiming = (pro.invertedSignal) ? (2) : (1);

  for (unsigned int i = firstDataTiming; i < changeCount - 1; i += 2) {
    code <<= 1;
    if (diff(KakuCC1101Component::timings[i], delay * pro.zero.high) < delayTolerance &&
        diff(KakuCC1101Component::timings[i + 1], delay * pro.zero.low) < delayTolerance) {
      // zero
    } else if (diff(KakuCC1101Component::timings[i], delay * pro.one.high) < delayTolerance &&
               diff(KakuCC1101Component::timings[i + 1], delay * pro.one.low) < delayTolerance) {
      // one
      code |= 1;
    } else {
      // Failed
      return false;
    }
  }

  if (changeCount > 7) {  // ignore very short transmissions: no device sends them, so this must be noise
    KakuCC1101Component::nReceivedValue = code;
    KakuCC1101Component::nReceivedBitlength = (changeCount - 1) / 2;
    KakuCC1101Component::nReceivedDelay = delay;
    KakuCC1101Component::nReceivedProtocol = p;
    return true;
  }

  return false;
}

void KakuCC1101Component::handleInterrupt() {
  static unsigned int changeCount = 0;
  static unsigned long lastTime = 0;
  static unsigned int repeatCount = 0;

  const long time = micros();
  const unsigned int duration = time - lastTime;

  if (duration > KakuCC1101Component::nSeparationLimit) {
    // A long stretch without signal level change occurred. This could
    // be the gap between two transmission.
    if ((repeatCount == 0) || (diff(duration, KakuCC1101Component::timings[0]) < 200)) {
      // This long signal is close in length to the long signal which
      // started the previously recorded timings; this suggests that
      // it may indeed by a a gap between two transmissions (we assume
      // here that a sender will send the signal multiple times,
      // with roughly the same gap between them).
      repeatCount++;
      if (repeatCount == 2) {
        for (unsigned int i = 1; i <= numProto; i++) {
          if (receiveProtocol(i, changeCount)) {
            // receive succeeded for protocol i
            break;
          }
        }
        repeatCount = 0;
      }
    }
    changeCount = 0;
  }

  // detect overflow
  if (changeCount >= RCSWITCH_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
  }

  KakuCC1101Component::timings[changeCount++] = duration;
  lastTime = time;
}

}  // namespace kaku_cc1101
}  // namespace esphome
