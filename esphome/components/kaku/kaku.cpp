#include "esphome/core/log.h"
#include "kaku.h"
#include "kaku_protocol.h"

namespace esphome {
namespace kaku {

static const char *TAG = "kaku";

void KakuComponent::dump_config() {}

float KakuComponent::get_setup_priority() const { return setup_priority::DATA; }

void KakuComponent::send(uint32_t address, uint8_t unit, bool on, float brightness) {
  auto call = transmitter->transmit(configurer);
  auto *data = call.get_data();
  data->reset();

  sync(*data);
  write_bits(*data, address, 26);
  write_bits(*data, 0, 1);  // ignore group

  if (on) {
    dim(*data);
  } else {
    write_bits(*data, 0, 1);
  }

  write_bits(*data, unit, 4);

  if (on) {
    write_bits(*data, (uint8_t)(brightness * 15.0f), 4);
  }

  pause(*data);

  ESP_LOGVV(TAG, ">--- DATA");
  for (const auto &n : data->get_data()) {
    ESP_LOGVV(TAG, "%d", n);
  }
  ESP_LOGVV(TAG, "<--- DATA");

  call.set_send_times(4);
  call.perform();
}

bool KakuComponent::on_receive(remote_base::RemoteReceiveData data) {
  data.reset();

  // sync
  if (!expect_sync(data))
    return false;

  ESP_LOGVV(TAG, "Sync found");

  //  ESP_LOGD(TAG, ">--- DATA IN");
  //  for (const auto n : *data.get_raw_data()) {
  //    ESP_LOGD(TAG, "%d", n);
  //  }
  //  ESP_LOGD(TAG, "<--- DATA IN");

  //  uint8_t tmp = 0;
  //  if (!set_bits(data, tmp)) {
  //    return false;
  //  }

  //  ESP_LOGD(TAG, "DATA");
  //  for (auto i = 0; i<64; i++) {
  //    ESP_LOGD(TAG, "%d", data.peek(i));
  //  }

  // address
  uint32_t address = 0;
  if (!set_bits(data, address, 26))
    return false;

  ESP_LOGD(TAG, "Address: %d", address);

  //  bool dim = false;
  //  if (!expect_dim(data)) {
  // group bit
  bool group = false;
  if (!set_bits(data, group))
    return false;

  ESP_LOGD(TAG, "Group: %d", group);

  bool on = false;
  if (!set_bits(data, on))
    return false;

  ESP_LOGD(TAG, "On/off: %s", on ? "on" : "off");
  //  } else {
  //    dim = true;
  //    ESP_LOGD(TAG, "Dim found");
  //  }

  uint8_t unit = 0;
  if (!set_bits(data, unit, 4))
    return false;

  ESP_LOGD(TAG, "Unit: %d", unit);

  //  uint8_t button_code = 0;
  //  if (!set_bits(data, button_code, 2))
  //    return false;
  //
  //  ESP_LOGD(TAG, "Button code: %d", button_code);

  //  if (dim) {
  //    uint8_t dim_value = 0;
  //    if (!set_bits(data, dim_value, 4))
  //      return false;
  //
  //    ESP_LOGD(TAG, "Dim level: %d", dim_value);
  //  }

  if (group) {
    for (auto *light : lights) {
      light->update_state(on);
    }
  } else {
    for (auto *light : lights) {
      if (light->get_unit() == unit) {
        light->update_state(on);
        break;
      }
    }
  }

  return true;
}

template<class T> bool KakuComponent::set_bits(remote_base::RemoteReceiveData &src, T &num, uint8_t size) const {
  for (auto i = 0; i < size; i++) {
    if (expect_one_manchester(src)) {
      num = (num << 1) | 1;
    } else if (expect_zero_manchester(src)) {
      num = (num << 1) | 0;
    } else {
      return false;
    }
  }
  return true;
}

template<class T> void KakuComponent::write_bits(remote_base::RemoteTransmitData &src, T num, uint8_t size) const {
  for (auto i = size - 1; i >= 0; i--) {
    if ((num >> i) & 1) {
      one_manchester(src);
    } else {
      zero_manchester(src);
    }
  }
}

bool KakuComponent::expect_one_manchester(remote_base::RemoteReceiveData &src) const {
  return expect_one(src) && expect_zero(src);
}

void KakuComponent::one_manchester(remote_base::RemoteTransmitData &src) const {
  one(src);
  zero(src);
}

bool KakuComponent::expect_zero_manchester(remote_base::RemoteReceiveData &src) const {
  return expect_zero(src) && expect_one(src);
}

void KakuComponent::zero_manchester(remote_base::RemoteTransmitData &src) const {
  zero(src);
  one(src);
}

bool KakuComponent::expect_dim(remote_base::RemoteReceiveData &src) const {
  for (auto i = 0; i < 4; i++) {
    auto index = i * 2;

    if (!src.peek_mark(250, index))
      return false;
    if (!src.peek_space(250, index + 1))
      return false;
  }
  src.advance(8);
  return true;
}

void KakuComponent::dim(remote_base::RemoteTransmitData &src) const {
  zero(src);
  zero(src);
}

bool KakuComponent::expect_zero(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(250, 1))
    return false;
  src.advance(2);
  return true;
}

void KakuComponent::zero(remote_base::RemoteTransmitData &src) const {
  src.mark(250);
  src.space(250);
}

bool KakuComponent::expect_one(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(1250, 1))
    return false;
  src.advance(2);
  return true;
}

void KakuComponent::one(remote_base::RemoteTransmitData &src) const {
  src.mark(250);
  src.space(1250);
}

bool KakuComponent::expect_sync(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(2500, 1))
    return false;
  src.advance(2);
  return true;
}

void KakuComponent::sync(remote_base::RemoteTransmitData &src) const {
  src.mark(250);
  src.space(2500);
}

void KakuComponent::pause(remote_base::RemoteTransmitData &src) const {
  src.mark(250);
  src.space(40 * 250);
}

void KakuLightComponent::setup_state(light::LightState *state) { this->state_ = state; }

void KakuLightComponent::update_state(bool on) {
  auto call = this->state_->make_call();
  call.set_state(on);
  call.perform();
  ignore = true;
}

light::LightTraits KakuLightComponent::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void KakuLightComponent::write_state(light::LightState *state) {
  if (!ignore) {
    parent->send(address, unit, state->current_values.is_on(), state->current_values.get_brightness());
  }
  ignore = false;
}

void KakuRemoteConfigurer::receive(cc1101::CC1101Component *component) { component->receive(); }

void KakuRemoteConfigurer::send(cc1101::CC1101Component *component) {
  component->send(cc1101::SERIAL_CONFIG, cc1101::SERIAL_PATABLE);  // TODO change to KAKU config instead of default
}

}  // namespace kaku
}  // namespace esphome
