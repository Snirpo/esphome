#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace cc1101 {

class CC1101Component : public Component,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_1MHZ> {
 public:
  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;

  void send_data(const std::vector<uint8_t> &data);
  std::vector<uint8_t> receive_data(uint8_t max_length);

  uint8_t write_command_strobe(uint8_t command);
  uint8_t sidle();
  void flush_rxfifo();
  void flush_txfifo();
  uint8_t receive();
  uint8_t send();

  uint8_t read_register(uint8_t address);
  std::vector<uint8_t> read_burst_register(uint8_t address, uint8_t max_length);

  void write_register(uint8_t address, uint8_t data);
  void write_burst_register(uint8_t address, const std::vector<uint8_t> &data);

  int16_t read_rssi();

 protected:
  void select_() { enable(); }
  void deselect_() { disable(); };

  uint8_t read_register_(uint8_t address);
  uint8_t read_register_with_sync_problem_(uint8_t address);

  void reset_(bool power_on_reset = false);
};

}  // namespace cc1101
}  // namespace esphome
