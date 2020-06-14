#pragma once

#include <esphome/components/light/light_output.h>
#include <esphome/components/remote_base/remote_base.h>
#include "esphome/core/component.h"

namespace esphome {
namespace kaku {

class KakuLightComponent;

class KakuLightUnit {
 public:
  KakuLightUnit(uint32_t address, uint8_t unit) : address(address), unit(unit) {}

  bool isUnit(uint32_t a, uint8_t u) const { return address == a && unit == u; }

  uint32_t getAddress() const { return address; }
  uint8_t getUnit() const { return unit; }

 private:
  uint32_t address;
  uint8_t unit;
};

class KakuMessage {
 public:
  KakuMessage(uint32_t address, bool group, bool on, uint8_t unit)
      : address(address), group(group), on(on), unit(unit) {}

  bool operator==(const KakuMessage& rhs) const {
    return address == rhs.address && group == rhs.group && on == rhs.on && unit == rhs.unit;
  }

  bool operator!=(const KakuMessage& rhs) const { return !(rhs == *this); }

 private:
  uint32_t address = 0;
  bool group = false;
  bool on = false;
  uint8_t unit = 0;
};

class KakuComponent : public Component, public remote_base::RemoteReceiverListener {
 public:
  void dump_config() override;
  float get_setup_priority() const override;

  bool on_receive(remote_base::RemoteReceiveData data) override;
  void send(uint32_t address, uint8_t unit, bool on, float brightness);

  void set_transmitter(remote_base::RemoteTransmitterBase* t) { transmitter = t; }
  void register_light(KakuLightComponent* component) { lights.push_back(component); }

 private:
  KakuMessage lastMessage = KakuMessage(0, false, 0, 0);
  remote_base::RemoteTransmitterBase* transmitter{nullptr};
  std::vector<KakuLightComponent*> lights;

  bool expect_dim(remote_base::RemoteReceiveData& src) const;
  bool expect_one(remote_base::RemoteReceiveData& src) const;
  bool expect_zero(remote_base::RemoteReceiveData& src) const;
  bool expect_sync(remote_base::RemoteReceiveData& src) const;
  bool expect_one_manchester(remote_base::RemoteReceiveData& src) const;
  bool expect_zero_manchester(remote_base::RemoteReceiveData& src) const;
  template<class T> bool set_bits(remote_base::RemoteReceiveData& src, T& data, uint8_t size = 1) const;

  void sync(remote_base::RemoteTransmitData& src) const;
  void pause(remote_base::RemoteTransmitData& src) const;
  void one(remote_base::RemoteTransmitData& src) const;
  void zero(remote_base::RemoteTransmitData& src) const;
  void dim(remote_base::RemoteTransmitData& src) const;
  void one_manchester(remote_base::RemoteTransmitData& src) const;
  void zero_manchester(remote_base::RemoteTransmitData& src) const;
  template<class T> void write_bits(remote_base::RemoteTransmitData& src, T num, uint8_t size) const;
};

class KakuLightComponent : public Component, public light::LightOutput {
 public:
  void update_state(bool on);
  void setup_state(light::LightState* state) override;
  light::LightTraits get_traits() override;
  void write_state(light::LightState* state) override;
  void set_kaku_parent(KakuComponent* component) {
    parent = component;
    component->register_light(this);
  }

  void add_group(uint32_t g) { groups.push_back(g); }
  void add_unit(KakuLightUnit u) { units.push_back(u); }

  bool equals(uint32_t a, uint8_t u) const {
    for (const auto& unit : units) {
      if (unit.isUnit(a, u)) {
        return true;
      }
    }
    return false;
  }

  bool equals(uint32_t a) const {
    for (const auto& address : groups) {
      if (address == a) {
        return true;
      }
    }
    return false;
  }

 private:
  KakuComponent* parent;
  std::vector<uint32_t> groups;
  std::vector<KakuLightUnit> units;
  light::LightState* state_;
  bool ignore = false;
};

}  // namespace kaku
}  // namespace esphome
