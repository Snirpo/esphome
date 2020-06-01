#include <EEPROM.h>
#include "esphome/core/log.h"
#include "somfy.h"
#include "esphome/core/preferences.h"

#define SYMBOL 640
#define UP 0x2
#define STOP 0x1
#define DOWN 0x4
#define PROG 0x8

namespace esphome {
namespace somfy {

static const char *TAG = "somfy";

void SomfyComponent::dump_config() {}

void SomfyComponent::setup() {}

float SomfyComponent::get_setup_priority() const { return setup_priority::DATA; }

uint32_t SomfyComponent::hash_base() { return 3104134497UL; }

void SomfyComponent::send(uint32_t address, uint16_t code, const cover::CoverCall &call) {
  auto frame =
      build_frame(address, code, call.get_stop() ? STOP : (call.get_position() == cover::COVER_OPEN ? DOWN : UP));

  send_command(frame, true);
  send_command(frame, false, 2);
}

void SomfyComponent::send_command(std::array<uint8_t, 7> frame, bool first, uint8_t count) {
  auto call = transmitter->transmit(configurer);
  auto *data = call.get_data();
  data->reset();

  if (first) {
    // Wake-up pulse & Silence, only with the first frame
    data->mark(9415);
    data->space(89565);

    // Hardware sync: two sync for the first frame
    for (int i = 0; i < 2; i++) {
      data->mark(4 * SYMBOL);
      data->space(4 * SYMBOL);
    }
  } else {
    // Hardware sync: seven for the following ones.
    for (int i = 0; i < 7; i++) {
      data->mark(4 * SYMBOL);
      data->space(4 * SYMBOL);
    }
  }

  // Software sync
  data->mark(4550);
  data->space(SYMBOL);

  // Data: bits are sent one by one, starting with the MSB.
  for (byte i = 0; i < 56; i++) {
    if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1) {
      data->space(SYMBOL);
      data->mark(SYMBOL);
    } else {
      data->mark(SYMBOL);
      data->space(SYMBOL);
    }
  }

  data->space(30415);  // Inter-frame silence

  call.set_send_times(count);
  call.perform();
}

std::array<uint8_t, 7> SomfyComponent::build_frame(uint32_t address, uint16_t code, uint8_t command) {
  auto frame = std::array<uint8_t, 7>();
  frame[0] = 0xA7;           // Encryption key. Doesn't matter much
  frame[1] = command << 4;   // Which button did  you press? The 4 LSB will be the checksum
  frame[2] = code >> 8;      // Rolling code (big endian)
  frame[3] = code;           // Rolling code
  frame[4] = address >> 16;  // Remote address
  frame[5] = address >> 8;   // Remote address
  frame[6] = address;        // Remote address

  // Checksum calculation: a XOR of all the nibbles
  uint8_t checksum = 0;
  for (byte i = 0; i < 7; i++) {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111;  // We keep the last 4 bits only

  // Checksum integration
  frame[1] |= checksum;  //  If a XOR of all the nibbles is equal to 0, the blinds will
  // consider the checksum ok.

  ESP_LOGVV(TAG, "Frame: %02X:%02X:%02X:%02X:%02X:%02X:%02X", frame[0], frame[1], frame[2], frame[3], frame[4],
            frame[5], frame[6]);

  // Obfuscation: a XOR of all the bytes
  for (byte i = 1; i < 7; i++) {
    frame[i] ^= frame[i - 1];
  }

  ESP_LOGVV(TAG, "Obfuscated frame: %02X:%02X:%02X:%02X:%02X:%02X:%02X", frame[0], frame[1], frame[2], frame[3],
            frame[4], frame[5], frame[6]);

  ESP_LOGD(TAG, "Sending from address %d with rolling code: %d", address, code);

  return frame;
}

void SomfyComponent::prog(uint32_t address, uint16_t code) {
  auto frame = build_frame(address, code, PROG);

  send_command(frame, true);
  send_command(frame, false, 2);
}

void SomfyCoverComponent::setup() {
  this->rolling_code_pref = global_preferences.make_preference<uint16_t>(this->get_object_id_hash(), true);
  uint16_t code = 1;
  if (!rolling_code_pref.load(&code)) {
    if (!rolling_code_pref.save(&code)) {
      return mark_failed();
    }
  }
}

cover::CoverTraits SomfyCoverComponent::get_traits() {
  auto traits = cover::CoverTraits();
  return traits;
}

void SomfyCoverComponent::control(const cover::CoverCall &call) {
  uint16_t code;
  rolling_code_pref.load(&code);
  code++;

  parent->send(address, code, call);

  rolling_code_pref.save(&code);
}

std::string SomfyCoverComponent::device_class() { return "blind"; }

void SomfyCoverComponent::prog() {
  uint16_t code = 1;
  parent->prog(address, code);
  rolling_code_pref.save(&code);
}

void SomfyRemoteConfigurer::send(cc1101::CC1101Component *component) {
  component->send(SOMFY_C1101_CONFIG, SOMFY_CC1101_PATABLE);
}

void SomfyRemoteConfigurer::receive(cc1101::CC1101Component *component) { component->receive(); }

}  // namespace somfy
}  // namespace esphome
