#pragma once

#include <unordered_map>
#include "esphome/components/cc1101/cc1101_reg.h"

namespace esphome {
namespace kaku_cc1101 {

using namespace cc1101;

static const std::vector<register_setting> KAKU_CONFIG{
    // FIFO mode
    {CC1101_IOCFG2, 0x01},   // Assert when RX FIFO is above threshold or at end of packet
    {CC1101_IOCFG0, 0x06},   // High-Z
    {CC1101_FIFOTHR, 0x47},  // ADC retention, 0dB RX attenuation, 5/60 FIFO threshold
    //{CC1101_SYNC1, 0x04},             // RX sync word, MSB //TODO
    //{CC1101_SYNC0, 0x00},             // RX sync word, LSB //TODO
    {CC1101_PKTLEN, 0xFF},    // Packet length
    {CC1101_PKTCTRL1, 0x00},  // All off
    {CC1101_PKTCTRL0, 0x02},  // Infinite packet length mode
    {CC1101_FSCTRL1, 0x0C},   // SmartRF
    {CC1101_FREQ2, 0x10},     // SmartRF
    {CC1101_FREQ1, 0xB0},     // SmartRF
    {CC1101_FREQ0, 0x71},     // SmartRF
    {CC1101_MDMCFG4, 0x2D},   // SmartRF
    {CC1101_MDMCFG3, 0x3B},   // SmartRF
    {CC1101_MDMCFG2, 0x30},   // OOK, no sync word
    //{CC1101_MDMCFG2, 0x32},   // OOK, 16 bit sync word
    //    {CC1101_MDMCFG1, 0x00},   // 8 bytes preamble
    {CC1101_DEVIATN, 0x62},  // SmartRF
    {CC1101_MCSM0, 0x18},    // Manual calibration
    {CC1101_FOCCFG, 0x1D},   // SmartRF
    {CC1101_BSCFG, 0x1C},
    {CC1101_AGCCTRL2, 0x04},  // SmartRF
    {CC1101_AGCCTRL1, 0x00},  // SmartRF
    {CC1101_AGCCTRL0, 0x92},  // SmartRF
    {CC1101_WORCTRL, 0xFB},   // SmartRF
    {CC1101_FREND1, 0xB6},    // PATABLE
    {CC1101_FREND0, 0x11},    // PATABLE
    {CC1101_FSCAL3, 0xEA},    // SmartRF
    {CC1101_FSCAL2, 0x2A},    // SmartRF
    {CC1101_FSCAL1, 0x00},    // SmartRF
    {CC1101_FSCAL0, 0x1F},    // SmartRF
    {CC1101_TEST2, 0x81},     // SmartRF
    {CC1101_TEST1, 0x35},     // SmartRF
    {CC1101_TEST0, 0x09},     // SmartRF
};

// Serial mode
//    {CC1101_FREQ2, 0x10},    {CC1101_FREQ1, 0xB0},    {CC1101_FREQ0, 0x71},    {CC1101_FSCTRL1, 0x06},
//    {CC1101_MDMCFG1, 0x03},  {CC1101_MDMCFG0, 0xFF},  {CC1101_CHANNR, 0x00},   {CC1101_DEVIATN, 0x47},
//    {CC1101_FREND1, 0x56},   {CC1101_MCSM0, 0x18},    {CC1101_FOCCFG, 0x16},   {CC1101_BSCFG, 0x1C},
//    {CC1101_AGCCTRL2, 0xC7}, {CC1101_AGCCTRL1, 0x00}, {CC1101_AGCCTRL0, 0xB2}, {CC1101_FSCAL3, 0xE9},
//    {CC1101_FSCAL2, 0x2A},   {CC1101_FSCAL1, 0x00},   {CC1101_FSCAL0, 0x1F},   {CC1101_FSTEST, 0x59},
//    {CC1101_TEST2, 0x81},    {CC1101_TEST1, 0x35},    {CC1101_TEST0, 0x09},    {CC1101_PKTCTRL1, 0x04},
//    {CC1101_ADDR, 0x00},     {CC1101_PKTLEN, 0x3D},   {CC1101_IOCFG2, 0x0D},   {CC1101_IOCFG0, 0x0D},
//    {CC1101_PKTCTRL0, 0x32}, {CC1101_MDMCFG3, 0x93},  {CC1101_MDMCFG4, 0x07},  {CC1101_MDMCFG2, 0x30},
//    {CC1101_FREND0, 0x11}};

/**
 * SmartRF preset for 10 dBm. Note some of the original values are invalid,
 * or should not be used.
 */
static const std::vector<uint8_t> KAKU_PATABLE = {0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// static const std::vector<uint8_t> KAKU_PATABLE = {0x6C, 0x1C, 0x06, 0x3A, 0x51, 0x85, 0xC8, 0xC0};

};  // namespace kaku_cc1101
}  // namespace esphome
