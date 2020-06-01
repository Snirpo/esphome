#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "cc1101_reg.h"

namespace esphome {
namespace cc1101 {

struct InterruptData {
  volatile bool data_available;
  volatile uint8_t count;
  ISRInternalGPIOPin *pin;

  void reset();
  static void gpio_intr(InterruptData *arg);
};

class CC1101Component : public Component,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_1MHZ> {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void send_data(const std::vector<uint8_t> &data);
  std::vector<uint8_t> receive_data(uint8_t max_length);

  uint8_t write_command_strobe(uint8_t command);
  uint8_t sidle();
  void flush_rxfifo();
  void flush_txfifo();
  uint8_t receive();
  uint8_t send(const std::vector<register_setting> &settings, const std::vector<uint8_t> &patable);
  uint8_t send();

  uint8_t read_register(uint8_t address);
  std::vector<uint8_t> read_burst_register(uint8_t address, uint8_t max_length);

  void write_register(uint8_t address, uint8_t data);
  void write_burst_register(uint8_t address, const std::vector<uint8_t> &data);

  int16_t read_rssi();

  void set_irq_pin(GPIOPin *pin) { irq_pin_ = pin; }

  bool data_available();

  void set_serial_mode(bool serial);

 protected:
  void select_() {
    enable();
    waitForMiso();
  }
  void deselect_() { disable(); };

  uint8_t read_register_(uint8_t address);
  uint8_t read_register_with_sync_problem_(uint8_t address);

  void reset_(bool power_on_reset = false);

  bool serial = false;
  GPIOPin *irq_pin_;
  InterruptData interruptData_;
};

class CC1101Device {
 public:
  void set_cc1101_parent(CC1101Component *cc1101) { this->cc1101_ = cc1101; }

 protected:
  CC1101Component *cc1101_{nullptr};
};

}  // namespace cc1101
}  // namespace esphome
