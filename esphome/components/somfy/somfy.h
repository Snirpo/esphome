#pragma once

#include <esphome/components/cover/cover.h>
#include <esphome/components/remote_base/remote_base.h>
#include "esphome/core/component.h"

namespace esphome {
namespace somfy {

class SomfyComponent : public Component, public Nameable {
 public:
  void dump_config() override;
  void setup() override;
  float get_setup_priority() const override;
  uint32_t hash_base() override;

  void send(uint32_t address, uint16_t code, const cover::CoverCall& call);
  void prog(uint32_t address, uint16_t code);

  void set_transmitter(remote_base::RemoteTransmitterBase* t) { transmitter = t; }

 private:
  remote_base::RemoteTransmitterBase* transmitter{nullptr};

  std::array<uint8_t, 7> build_frame(uint32_t address, uint16_t code, uint8_t command);
  void send_command(std::array<uint8_t, 7>, bool first, uint8_t count = 1);
};

class SomfyCoverComponent : public Component, public cover::Cover {
 public:
  void setup();
  cover::CoverTraits get_traits() override;
  void control(const cover::CoverCall& call) override;
  std::string device_class() override;
  void set_somfy_parent(SomfyComponent* component) { parent = component; }
  void set_address(uint32_t a) { address = a; }
  void prog();

 private:
  SomfyComponent* parent;
  ESPPreferenceObject rolling_code_pref;
  uint32_t address;
};

template<typename... Ts> class ProgAction : public Action<Ts...> {
 public:
  explicit ProgAction(SomfyCoverComponent* component) : component_(component) {}

  void play(Ts... x) override { this->component_->prog(); }

 protected:
  SomfyCoverComponent* component_;
};

}  // namespace somfy
}  // namespace esphome
