#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/fan/fan_state.h"
#include "esphome/components/cc1101/cc1101.h"

namespace esphome {
namespace itho_ecofanrft {

class IthoEcoFanRftComponent;

struct IthoEcoFanRftComponentStore {
  volatile bool data_available;
  volatile uint8_t count;
  ISRInternalGPIOPin *pin;

  void reset();
  static void gpio_intr(IthoEcoFanRftComponentStore *arg);
};

class IthoEcoFanRftComponent : public fan::FanState {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_irq_pin(GPIOPin *irq) { irq_ = irq; }

  void set_rf_address(uint64_t address) {
    this->rf_address_.resize(3, 0);
    for (uint8_t i = 0; i < 3; i++) {
      this->rf_address_[i] = (address >> (8 * (2 - i))) & 0xff;
    }
  }

  void set_peer_rf_address(uint64_t address) {
    this->peer_rf_address_.resize(3, 0);
    for (uint8_t i = 0; i < 3; i++) {
      this->peer_rf_address_[i] = (address >> (8 * (2 - i))) & 0xff;
    }
  }

  void join();
  void send_command(std::string command);

 private:
  GPIOPin *irq_;
  std::vector<uint8_t> rf_address_;
  std::vector<uint8_t> peer_rf_address_;
  IthoEcoFanRftComponentStore store_;
  cc1101::CC1101Component *cc1101_{nullptr};
  bool next_update_{true};
  std::vector<uint8_t> packet_;
  std::uint8_t counter_ = 0;
  std::uint8_t tries_ = 0;

  std::string format_addr_(std::vector<uint8_t> addr);
  void schedule_send_packet_();
  void init_itho();
  void enable_receive_mode() {
    this->cc1101_->flush_rxfifo();
    this->cc1101_->receive();
  }
  std::vector<uint8_t> get_data();
  bool get_fan_speed(std::vector<uint8_t> peer_rf_address, uint8_t *speed);
  bool has_valid_checksum(std::vector<uint8_t> data) { return this->calc_checksum(data) == 0x00; }
  uint8_t calc_checksum(std::vector<uint8_t> data);
  uint8_t send_packet();
  void prepare_packet(std::string command);
};

template<typename... Ts> class JoinAction : public Action<Ts...> {
 public:
  explicit JoinAction(IthoEcoFanRftComponent *component) : component_(component) {}

  void play(Ts... x) override { this->component_->join(); }

 protected:
  IthoEcoFanRftComponent *component_;
};

}  // namespace itho_ecofanrft
}  // namespace esphome
