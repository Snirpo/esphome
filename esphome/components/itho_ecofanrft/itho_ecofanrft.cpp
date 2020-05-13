#include "esphome/core/log.h"
#include "itho_ecofanrft.h"
#include "itho_cc1101_protocol.h"

namespace esphome {
namespace itho_ecofanrft {

static const char *TAG = "itho_ecofanrft";

std::string IthoEcoFanRftComponent::format_addr_(std::vector<uint8_t> addr) {
  std::string s;
  char buf[20];
  for (uint8_t a : addr) {
    sprintf(&buf[0], "%02X:", a);
    s += buf;
  }
  s.pop_back();
  return s;
}

void itho_ecofanrft::IthoEcoFanRftComponent::dump_config() {
  std::string s;

  ESP_LOGCONFIG(TAG, "Itho EcoFanRft '%s'", this->get_name().c_str());
  ESP_LOGCONFIG(TAG, "  RF Address: '%s'", this->format_addr_(this->rf_address_).c_str());
  if (this->peer_rf_address_.size() > 0) {
    ESP_LOGCONFIG(TAG, "  RF Peer Address: '%s'", this->format_addr_(this->peer_rf_address_).c_str());
  }

#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
  std::vector<uint8_t> config = this->cc1101_->read_burst_register(0x00, 47);
  for (uint8_t i = 0; i < config.size(); i++) {
    ESP_LOGCONFIG(TAG, "Config register [%02X] => [%02X]", i, config[i]);
  }
#endif
}
void IthoEcoFanRftComponent::setup() {
  if (!this->cc1101_->is_failed()) {
    this->mark_failed();
    return;
  }

  // Setup module for Itho protocol
  this->init_itho();

  auto traits = fan::FanTraits(false, true);  // No oscillating, just speed
  this->set_traits(traits);
  this->add_on_state_callback([this]() { this->next_update_ = true; });

  // Set CC1101 in receive mode
  this->enable_receive_mode();
}
void IthoEcoFanRftComponent::loop() {
  if (this->cc1101_->data_available()) {
    {
      uint8_t speed;
      if (this->get_fan_speed(this->peer_rf_address_, &speed)) {
        auto call = this->make_call();

        if (speed > 0x00) {
          call.set_state(true);

          if (speed < 0x40) {
            call.set_speed(fan::FAN_SPEED_LOW);
          } else if (speed < 0x80) {
            call.set_speed(fan::FAN_SPEED_MEDIUM);
          } else {
            call.set_speed(fan::FAN_SPEED_HIGH);
          }
        } else {
          call.set_state(false);
        }
        call.perform();

        // next_update should not run after RF status update
        this->next_update_ = false;
      }
    }
    this->enable_receive_mode();
  }

  if (!this->next_update_) {
    return;
  }
  this->next_update_ = false;

  {
    std::string speed;

    if (this->state) {
      if (this->speed == fan::FAN_SPEED_LOW)
        speed = "low";
      else if (this->speed == fan::FAN_SPEED_MEDIUM)
        speed = "medium";
      else if (this->speed == fan::FAN_SPEED_HIGH)
        speed = "high";
    }
    ESP_LOGD(TAG, "Setting speed: '%s'", speed.c_str());

    bool enable = this->state;
    if (enable) {
      ESP_LOGD(TAG, "Sending speed: '%s'", speed.c_str());
      this->send_command(speed);
    } else {
      ESP_LOGD(TAG, "Sending speed: min");
      this->send_command("min");
    }

    ESP_LOGD(TAG, "Setting itho_ecofanrft state: %s", ONOFF(enable));
  }
}
float IthoEcoFanRftComponent::get_setup_priority() const { return setup_priority::DATA; }

void IthoEcoFanRftComponent::join() {
  ESP_LOGD(TAG, "Fan '%s': join() called", this->get_name().c_str());
  this->send_command("join");
}

void IthoEcoFanRftComponent::send_command(std::string command) {
  this->prepare_packet(command);
  this->send_packet();

  // After sending command switch back to receive mode
  this->enable_receive_mode();

  this->set_timeout("send_command", 40, [this]() { this->schedule_send_packet_(); });
}

void IthoEcoFanRftComponent::schedule_send_packet_() {
  uint8_t tries_left = this->send_packet();

  // After sending packet switch back to receive mode
  this->enable_receive_mode();

  if (tries_left > 0) {
    this->set_timeout("send_command", 50, [this]() { this->schedule_send_packet_(); });
  }
}

void IthoEcoFanRftComponent::init_itho() {
  for (register_setting r : ITHO_CC1101_CONFIG) {
    this->cc1101_->write_register(r.address, r.data);
  }
  this->cc1101_->write_burst_register(CC1101_PATABLE, ITHO_CC1101_PATABLE);

  this->cc1101_->write_command_strobe(CC1101_SCAL);
  delay(1);
}

std::vector<uint8_t> IthoEcoFanRftComponent::get_data() {
  std::vector<uint8_t> out;
  std::vector<uint8_t> data = this->cc1101_->receive_data(MAX_PACKET_LEN);

  auto pos =
      std::search(data.begin(), data.end(), packet::ITHO_CC1101_HEADER.begin(), packet::ITHO_CC1101_HEADER.end());
  if (pos != data.end()) {
    uint8_t offset = pos - data.begin();
    ESP_LOGV(TAG, "Found Itho header at offset: %d", offset);

    auto end =
        std::find_first_of(pos, data.end(), packet::ITHO_CC1101_FOOTER.begin(), packet::ITHO_CC1101_FOOTER.end());

    if (end != data.end()) {
      uint8_t endoffset = end - pos;
      uint8_t packet_size = end - pos - packet::ITHO_CC1101_HEADER.size();
      ESP_LOGV(TAG, "Found Itho footer at offset: %d", endoffset);
      ESP_LOGV(TAG, "Packet size: %d", packet_size);

      std::vector<bool> raw1, raw2, raw3;
      raw1.resize((endoffset - 5) * 8, false);
      for (uint8_t i = 0; i < (endoffset - 5); i++) {
        uint8_t c = data[i + 5 + offset];
        for (uint8_t j = 0; j < 8; j++) {
          uint16_t idx = i * 8 + 7 - j;
          bool v = (c >> j) & 1;
          raw1[idx] = v;
        }
      }

      raw2.resize(raw1.size() / 2, false);
      for (uint16_t i = 0; i < raw2.size(); i++) {
        raw2[i] = raw1[i * 2];
      }

      // div into group of 5, drop last bit and reverse
      unsigned int nf = (raw2.size() / 5);
      unsigned int fl = nf * 4;
      raw3.resize(fl, false);
      for (unsigned int i = 0; i < nf; i++) {
        for (unsigned int j = 0; j < 4; j++) {
          raw3[(i * 4 + j)] = raw2[(i * 5 + (3 - j))];
        }
      }

      out.resize(raw3.size() / 8, 0);

      for (unsigned int i = 0; i < out.size(); i++) {
        uint8_t x;
        x = raw3[i * 8] << 7 | raw3[i * 8 + 1] << 6 | raw3[i * 8 + 2] << 5 | raw3[i * 8 + 3] << 4;
        x |= raw3[i * 8 + 4] << 3 | raw3[i * 8 + 5] << 2 | raw3[i * 8 + 6] << 1 | raw3[i * 8 + 7];
        out[i] = x;
      }

#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
      for (uint8_t i : out) {
        ESP_LOGVV(TAG, "Packet payload (%02X)", i);
      }
#endif

      uint8_t checksum = 0;
      for (unsigned char i : out) {
        checksum = (checksum + i) & 0xFF;
      }
      ESP_LOGV(TAG, "Packet Checksum: 0x%02X (%s)", checksum, checksum ? "invalid" : "valid");

    }  // Footer
  }    // Header
  return out;
}

bool IthoEcoFanRftComponent::get_fan_speed(std::vector<uint8_t> peer_rf_address, uint8_t *speed) {
  std::vector<uint8_t> data = this->get_data();
  *speed = 0xFF;

  if (has_valid_checksum(data)) {
    auto id = std::search(data.begin(), data.end(), peer_rf_address.begin(), peer_rf_address.end());
    auto pos = std::search(data.begin(), data.end(), ITHO_STATUS.begin(), ITHO_STATUS.end());
    if (data.size() == 11 &&  // 1 + 3 + 5 + 1 + 1
        data[0] == 0x14 && pos - data.begin() == 4) {
      pos += ITHO_STATUS.size();
      *speed = *pos;

      if (!peer_rf_address.empty() && id - data.begin() != 1) {
        // Not our peer rf address
        return false;
      }

      ESP_LOGD(TAG, "Received fan speed status: peer rf address %02X:%02X:%02X, speed 0x%02X", data[1], data[2],
               data[3], *speed);

      if (peer_rf_address.empty()) {
        ESP_LOGI(TAG, "No peer rf address configured, ignoring above status!");
        return false;
      }
      return true;
    }
  }

  return false;
}

uint8_t IthoEcoFanRftComponent::calc_checksum(std::vector<uint8_t> data) {
  uint8_t checksum = 0;
  for (unsigned char i : data) {
    checksum = (checksum + i) & 0xFF;
  }
  return 0 - checksum;
}

void IthoEcoFanRftComponent::prepare_packet(std::string command) {
  auto cmd_it = ITHO_COMMANDS.find(command);
  if (cmd_it == ITHO_COMMANDS.end()) {
    ESP_LOGD(TAG, "Unknown command '%s'", command.c_str());
    return;
  }

  this->tries_ = 3;

  std::vector<uint8_t> cmd = packet::INDICATOR_COMMAND;
  cmd.insert(cmd.end(), this->rf_address_.begin(), this->rf_address_.end());
  cmd.push_back(this->counter_++);
  cmd.insert(cmd.end(), cmd_it->second.begin(), cmd_it->second.end());

  if (command == "join") {
    cmd.insert(cmd.end(), this->rf_address_.begin(), this->rf_address_.end());
    cmd.insert(cmd.end(), ITHO_COMMANDS.at("join_2").begin(), ITHO_COMMANDS.at("join_2").end());
    cmd.insert(cmd.end(), this->rf_address_.begin(), this->rf_address_.end());
  } else if (command == "leave") {
    cmd.insert(cmd.end(), this->rf_address_.begin(), this->rf_address_.end());
  }

  cmd.push_back(this->calc_checksum(cmd));

  std::vector<bool> raw1, raw2;
  raw1.resize(((5 * cmd.size() * 2 + 4 - 1) & -4),
              true);  // 2 nibbles per byte, 5 bits per nibble, round up to multiple of 4
  uint16_t idx;
  bool v;
  for (uint8_t i = 0; i < cmd.size(); i++) {
    for (uint8_t j = 0; j < 2; j++) {
      uint8_t n = cmd[i] >> (4 * (1 - j));
      for (uint8_t k = 0; k < 4; k++) {
        idx = 5 * (2 * i + j) + k;
        v = (n >> k) & 1;
        raw1[idx] = v;
      }
      // raw1[5 * (2 * i + j) + 4] = true;  //is already true
    }
  }

  // manchester encode
  raw2.resize(2 * raw1.size());
  for (uint16_t i = 0; i < raw1.size(); i++) {
    raw2[(2 * i)] = raw1[i];
    raw2[(2 * i) + 1] = !raw1[i];
  }

  std::vector<uint8_t> out;
  out.resize(raw2.size() / 8, 0);

  for (unsigned int i = 0; i < out.size(); i++) {
    uint8_t x;
    x = raw2[i * 8] << 7 | raw2[i * 8 + 1] << 6 | raw2[i * 8 + 2] << 5 | raw2[i * 8 + 3] << 4;
    x |= raw2[i * 8 + 4] << 3 | raw2[i * 8 + 5] << 2 | raw2[i * 8 + 6] << 1 | raw2[i * 8 + 7];
    out[i] = x;
  }

  this->packet_ = packet::ITHO_CC1101_HEADER;
  // std::vector<uint8_t> this->packet_ = packet::ITHO_CC1101_HEADER;
  this->packet_.insert(this->packet_.end(), out.begin(), out.end());
  if (cmd.size() % 2 == 0) {
    this->packet_.push_back(packet::FOOTER_EVEN);
  } else {
    this->packet_.push_back(packet::FOOTER_ODD);
  }
  while (this->packet_.size() < MAX_PACKET_LEN) {
    this->packet_.push_back(packet::POSTAMBLE);
  }
}

uint8_t IthoEcoFanRftComponent::send_packet() {
  ESP_LOGV(TAG, "%d tries left for sending packet of %d bytes", this->tries_, this->packet_.size());
  if (this->packet_.size() > 0 && this->tries_-- > 0) {
    this->cc1101_->send_data(this->packet_);
  }

  return this->tries_;
}

}  // namespace itho_ecofanrft
}  // namespace esphome
