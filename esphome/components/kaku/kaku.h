#pragma once

#include <esphome/components/light/light_output.h>
#include <esphome/components/remote_base/remote_base.h>
#include "esphome/core/component.h"

namespace esphome {
namespace kaku {

class KakuComponent : public Component, public light::LightOutput, public remote_base::RemoteReceiverListener {
 public:
  void dump_config() override;
  float get_setup_priority() const override;

  light::LightTraits get_traits() override;
  void write_state(light::LightState* state) override;
  bool on_receive(remote_base::RemoteReceiveData data) override;

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
  template<class T> void write_bits(remote_base::RemoteTransmitData& src, T& num, uint8_t size) const;
};

}  // namespace kaku
}  // namespace esphome
