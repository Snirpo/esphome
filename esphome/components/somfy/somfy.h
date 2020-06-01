#pragma once

#include <esphome/components/cover/cover.h>
#include <esphome/components/remote_base/remote_base.h>
#include <esphome/components/cc1101/cc1101.h>
#include "esphome/core/component.h"

namespace esphome {
namespace somfy {

static const std::vector<cc1101::register_setting> SOMFY_C1101_CONFIG{
    {cc1101::CC1101_FREQ2, 0x10},    {cc1101::CC1101_FREQ1, 0xAB},    {cc1101::CC1101_FREQ0, 0xA5},
    {cc1101::CC1101_FSCTRL0, 0x00},  {cc1101::CC1101_FSCTRL1, 0x06},  {cc1101::CC1101_MDMCFG1, 0x23},
    {cc1101::CC1101_MDMCFG0, 0xB9},  {cc1101::CC1101_CHANNR, 0x00},   {cc1101::CC1101_DEVIATN, 0x00},
    {cc1101::CC1101_FREND1, 0x56},   {cc1101::CC1101_MCSM2, 0x07},    {cc1101::CC1101_MCSM1, 0x00},
    {cc1101::CC1101_MCSM0, 0x18},    {cc1101::CC1101_FOCCFG, 0x14},   {cc1101::CC1101_BSCFG, 0x6C},
    {cc1101::CC1101_AGCCTRL2, 0x07}, {cc1101::CC1101_AGCCTRL1, 0x00}, {cc1101::CC1101_AGCCTRL0, 0x90},
    {cc1101::CC1101_FSCAL3, 0xE9},   {cc1101::CC1101_FSCAL2, 0x2A},   {cc1101::CC1101_FSCAL1, 0x00},
    {cc1101::CC1101_FSCAL0, 0x1F},   {cc1101::CC1101_FSTEST, 0x59},   {cc1101::CC1101_TEST2, 0x81},
    {cc1101::CC1101_TEST1, 0x35},    {cc1101::CC1101_TEST0, 0x09},    {cc1101::CC1101_PKTCTRL1, 0x04},
    {cc1101::CC1101_ADDR, 0x00},     {cc1101::CC1101_PKTLEN, 0x3D},   {cc1101::CC1101_IOCFG2, 0x0D},
    {cc1101::CC1101_IOCFG1, 0x2E},   {cc1101::CC1101_IOCFG0, 0x2D},   {cc1101::CC1101_FIFOTHR, 0x07},
    {cc1101::CC1101_PKTCTRL0, 0x32}, {cc1101::CC1101_MDMCFG3, 0x0A},  {cc1101::CC1101_MDMCFG4, 0x55},
    {cc1101::CC1101_MDMCFG2, 0x30},  {cc1101::CC1101_FREND0, 0x11},   {cc1101::CC1101_SYNC1, 0xD3},
    {cc1101::CC1101_SYNC0, 0x91},    {cc1101::CC1101_ADDR, 0x00},     {cc1101::CC1101_CHANNR, 0x00},
    {cc1101::CC1101_WOREVT1, 0x87},  {cc1101::CC1101_WOREVT0, 0x6B},  {cc1101::CC1101_WORCTRL, 0xF8},
    {cc1101::CC1101_RCCTRL1, 0x41},  {cc1101::CC1101_RCCTRL0, 0x00}};

static const std::vector<uint8_t> SOMFY_CC1101_PATABLE = {0x00, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

class SomfyCC1101Configurer : public cc1101::CC1101Configurer {
 public:
  void receive() override;
  void send() override;
};

class SomfyComponent : public Component, public Nameable {
 public:
  void dump_config() override;
  void setup() override;
  float get_setup_priority() const override;
  uint32_t hash_base() override;

  void send(uint32_t address, uint16_t code, const cover::CoverCall& call);
  void prog(uint32_t address, uint16_t code);

  void set_transmitter(remote_base::RemoteTransmitterBase* t) { transmitter = t; }

  // TODO: generic
  void set_configurer(cc1101::CC1101Component* c) {
    configurer = new SomfyCC1101Configurer();
    configurer->set_cc1101(c);
  }

 private:
  SomfyCC1101Configurer* configurer;
  remote_base::RemoteTransmitterBase* transmitter{nullptr};

  std::array<uint8_t, 7> build_frame(uint32_t address, uint16_t code, uint8_t command);
  void send_command(std::array<uint8_t, 7>, bool first, uint8_t count = 1);
};

class SomfyCoverComponent : public Component, public cover::Cover {
 public:
  void setup();
  cover::CoverTraits get_traits() override;
  void control(const cover::CoverCall& call) override;
  std::string device_class() override;
  void set_somfy_parent(SomfyComponent* component) { parent = component; }
  void set_address(uint32_t a) { address = a; }
  void prog();

 private:
  SomfyComponent* parent;
  ESPPreferenceObject rolling_code_pref;
  uint32_t address;
};

template<typename... Ts> class ProgAction : public Action<Ts...> {
 public:
  explicit ProgAction(SomfyCoverComponent* component) : component_(component) {}

  void play(Ts... x) override { this->component_->prog(); }

 protected:
  SomfyCoverComponent* component_;
};

}  // namespace somfy
}  // namespace esphome
