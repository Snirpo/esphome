#include "esphome/core/log.h"
#include "kaku.h"
#include "kaku_protocol.h"

namespace esphome {
namespace kaku {

static const char *TAG = "kaku";

void KakuComponent::dump_config() {}

float KakuComponent::get_setup_priority() const { return setup_priority::DATA; }

light::LightTraits KakuComponent::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void KakuComponent::write_state(light::LightState *state) {}

bool KakuComponent::on_receive(remote_base::RemoteReceiveData data) {
  ESP_LOGD(TAG, "Parsing message");

  data.reset();

  // sync
  if (!expect_sync(data))
    return false;

  // address
  uint32_t address = 0;
  if (!set_bits(data, address, 26))
    return false;

  // group bit
  uint8_t group = 0;
  if (!set_bits(data, group))
    return false;

  bool dim = false;
  bool on = true;
  if (expect_dim(data)) {
    dim = true;
  } else if (!set_bits(data, on)) {
    return false;
  }

  uint8_t unit = 0;
  if (!set_bits(data, unit, 4))
    return false;

  uint8_t dim_val = 0;
  if (dim) {
    if (!set_bits(data, dim_val, 4))
      return false;
  }

#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
  ESP_LOGVV(TAG, "Address: %d", address);
  ESP_LOGVV(TAG, "Group: %d", group);
  ESP_LOGVV(TAG, "Unit: %d", unit);
  if (dim) {
    ESP_LOGVV(TAG, "Dim: %d", dim_val);
  } else {
    ESP_LOGVV(TAG, "On/off: %s", on ? "on" : "off");
  }
#endif

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
}

bool KakuComponent::expect_dim(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(250))
    return false;
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(250))
    return false;
}

bool KakuComponent::expect_one_manchester(remote_base::RemoteReceiveData &src) const {
  return expect_one(src) && expect_zero(src);
}

bool KakuComponent::expect_zero_manchester(remote_base::RemoteReceiveData &src) const {
  return expect_zero(src) && expect_one(src);
}

bool KakuComponent::expect_one(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(250))
    return false;
  src.advance(2);
  return true;
}

bool KakuComponent::expect_zero(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(1250))
    return false;
  src.advance(2);
  return true;
}

bool KakuComponent::expect_sync(remote_base::RemoteReceiveData &src) const {
  if (!src.peek_mark(250))
    return false;
  if (!src.peek_space(2500, 1))
    return false;
  src.advance(2);
  return true;
}

}  // namespace kaku
}  // namespace esphome
