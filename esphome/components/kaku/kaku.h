#pragma once

#include <esphome/components/light/light_output.h>
#include <esphome/components/remote_base/remote_base.h>
#include "esphome/core/component.h"

namespace esphome {
namespace kaku {

class KakuComponent : public Component, public remote_base::RemoteReceiverListener {
 public:
  void dump_config() override;
  float get_setup_priority() const override;

  bool on_receive(remote_base::RemoteReceiveData data) override;
  void send(uint32_t address, uint8_t unit, bool on, float brightness);

  void set_transmitter(remote_base::RemoteTransmitterBase* t) { transmitter = t; }

 private:
  remote_base::RemoteTransmitterBase* transmitter{nullptr};

  bool expect_dim(remote_base::RemoteReceiveData& src) const;
  bool expect_one(remote_base::RemoteReceiveData& src) const;
  bool expect_zero(remote_base::RemoteReceiveData& src) const;
  bool expect_sync(remote_base::RemoteReceiveData& src) const;
  bool expect_one_manchester(remote_base::RemoteReceiveData& src) const;
  bool expect_zero_manchester(remote_base::RemoteReceiveData& src) const;
  template<class T> bool set_bits(remote_base::RemoteReceiveData& src, T& data, uint8_t size = 1) const;

  void sync(remote_base::RemoteTransmitData& src) const;
  void one(remote_base::RemoteTransmitData& src) const;
  void zero(remote_base::RemoteTransmitData& src) const;
  void dim(remote_base::RemoteTransmitData& src) const;
  void one_manchester(remote_base::RemoteTransmitData& src) const;
  void zero_manchester(remote_base::RemoteTransmitData& src) const;
  template<class T> void write_bits(remote_base::RemoteTransmitData& src, T num, uint8_t size) const;
};

class KakuLightComponent : public Component, public light::LightOutput {
 public:
  light::LightTraits get_traits() override;
  void write_state(light::LightState* state) override;
  void set_kaku_parent(KakuComponent* component) { parent = component; }
  void set_address(uint32_t a) { address = a; }
  void set_unit(uint8_t u) { unit = u; }

 private:
  KakuComponent* parent;
  uint32_t address;
  uint8_t unit = 0;
};

}  // namespace kaku
}  // namespace esphome
