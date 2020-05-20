#pragma once

#include <esphome/components/light/light_output.h>
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/fan/fan_state.h"
#include "esphome/components/cc1101/cc1101.h"

namespace esphome {
namespace kaku_cc1101 {

class KakuCC1101Component : public Component, public light::LightOutput, public cc1101::CC1101Device {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  light::LightTraits get_traits() override;
  void write_state(light::LightState *state) override;

  void set_address(uint32_t a) { address = a; };

 private:
  uint32_t address;
  std::vector<uint8> packet_;
  std::uint8_t tries_ = 0;

  uint8_t send_packet();
};

}  // namespace kaku_cc1101
}  // namespace esphome
