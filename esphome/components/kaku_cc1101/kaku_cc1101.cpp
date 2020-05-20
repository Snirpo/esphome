#include "esphome/core/log.h"
#include "kaku_cc1101.h"
#include "kaku_protocol.h"

namespace esphome {
namespace kaku_cc1101 {

static const char *TAG = "kaku_cc1101";

void KakuCC1101Component::dump_config() {
#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
  std::vector<uint8_t> config = this->cc1101_->read_burst_register(0x00, 47);
  for (uint8_t i = 0; i < config.size(); i++) {
    ESP_LOGCONFIG(TAG, "Config register [%02X] => [%02X]", i, config[i]);
  }
#endif
}
void KakuCC1101Component::setup() {
  for (register_setting r : KAKU_CONFIG) {
    this->cc1101_->write_register(r.address, r.data);
  }
  this->cc1101_->write_burst_register(CC1101_PATABLE, KAKU_PATABLE);

  this->cc1101_->write_command_strobe(CC1101_SCAL);

  this->cc1101_->send();
  delay(1);
}

float KakuCC1101Component::get_setup_priority() const { return setup_priority::DATA; }

uint8_t KakuCC1101Component::send_packet() {
  ESP_LOGV(TAG, "%d tries left for sending packet of %d bytes", this->tries_, this->packet_.size());
  if (!this->packet_.empty() && this->tries_-- > 0) {
    this->cc1101_->send_data(this->packet_);
  }

  if (tries_ > 0) {
    this->set_timeout("send_packet", 50, [this]() { this->send_packet(); });
  }
}

light::LightTraits KakuCC1101Component::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void KakuCC1101Component::write_state(light::LightState *state) {
  bool manchester = true;

  packet_.clear();

  packet_.push_back(high);
  for (uint8_t i = 0; i < 10; i++) {
    packet_.push_back(low);
  }

  if (manchester) {
    //uint8_t high = 0b00000010; // 1 high, 1 low, length = 2
    //uint8_t low = 0b00100000; // 1 high, 5 low, length = 6

    // Manchester encoded
    uint8_t high = 0b10100000; // 1 high, 1 low, length = 2
    uint8_t low = 0b10000010; // 1 high, 5 low, length = 6
    uint8_t dim = 0b1010;

    uint64_t out = 0;

    for (uint8_t i = 25; i >= 0; i--) {
      if ((address >> i) & 1) {
        packet_.push_back(high);
        packet_.push_back(low);
      }
      else {
        packet_.push_back(low);
        packet_.push_back(high);
      }
    }
  }
  else {
    uint8_t low = 0b10100000;
    uint8_t high = 0b10000010;
    uint8_t dim = 0b1010;

    for (uint8_t i = 25; i >= 0; i--) {
      packet_.push_back((address >> i) & 1 ? high : low);
    }

    packet_.push_back(low);                                         // group number
    packet_.push_back(state->current_values.is_on() ? high : low);  // on/off/dim

    uint8_t light = 0;
    for (uint8_t i = 3; i >= 0; i--) {
      packet_.push_back((light >> i) | 1 ? high : low);
    }
  }
}

}  // namespace kaku_cc1101
}  // namespace esphome
