#pragma once

namespace esphome {
namespace cc1101 {

struct register_setting {
  uint8_t address;
  uint8_t data;
};

/* Type of transfers */
static const uint8_t CC1101_WRITE_SINGLE = 0x00;
static const uint8_t CC1101_WRITE_BURST = 0x40;
static const uint8_t CC1101_READ_SINGLE = 0x80;
static const uint8_t CC1101_READ_BURST = 0xC0;

/* CC1101 configuration registers */
static const uint8_t CC1101_IOCFG2 = 0x00;    // GDO2 Output Pin Configuration
static const uint8_t CC1101_IOCFG1 = 0x01;    // GDO1 Output Pin Configuration
static const uint8_t CC1101_IOCFG0 = 0x02;    // GDO0 Output Pin Configuration
static const uint8_t CC1101_FIFOTHR = 0x03;   // RX FIFO and TX FIFO Thresholds
static const uint8_t CC1101_SYNC1 = 0x04;     // Sync Word, High Byte
static const uint8_t CC1101_SYNC0 = 0x05;     // Sync Word, Low Byte
static const uint8_t CC1101_PKTLEN = 0x06;    // Packet Length
static const uint8_t CC1101_PKTCTRL1 = 0x07;  // Packet Automation Control
static const uint8_t CC1101_PKTCTRL0 = 0x08;  // Packet Automation Control
static const uint8_t CC1101_ADDR = 0x09;      // Device Address
static const uint8_t CC1101_CHANNR = 0x0A;    // Channel Number
static const uint8_t CC1101_FSCTRL1 = 0x0B;   // Frequency Synthesizer Control
static const uint8_t CC1101_FSCTRL0 = 0x0C;   // Frequency Synthesizer Control
static const uint8_t CC1101_FREQ2 = 0x0D;     // Frequency Control Word, High Byte
static const uint8_t CC1101_FREQ1 = 0x0E;     // Frequency Control Word, Middle Byte
static const uint8_t CC1101_FREQ0 = 0x0F;     // Frequency Control Word, Low Byte
static const uint8_t CC1101_MDMCFG4 = 0x10;   // Modem Configuration
static const uint8_t CC1101_MDMCFG3 = 0x11;   // Modem Configuration
static const uint8_t CC1101_MDMCFG2 = 0x12;   // Modem Configuration
static const uint8_t CC1101_MDMCFG1 = 0x13;   // Modem Configuration
static const uint8_t CC1101_MDMCFG0 = 0x14;   // Modem Configuration
static const uint8_t CC1101_DEVIATN = 0x15;   // Modem Deviation Setting
static const uint8_t CC1101_MCSM2 = 0x16;     // Main Radio Control State Machine Configuration
static const uint8_t CC1101_MCSM1 = 0x17;     // Main Radio Control State Machine Configuration
static const uint8_t CC1101_MCSM0 = 0x18;     // Main Radio Control State Machine Configuration
static const uint8_t CC1101_FOCCFG = 0x19;    // Frequency Offset Compensation Configuration
static const uint8_t CC1101_BSCFG = 0x1A;     // Bit Synchronization Configuration
static const uint8_t CC1101_AGCCTRL2 = 0x1B;  // AGC Control
static const uint8_t CC1101_AGCCTRL1 = 0x1C;  // AGC Control
static const uint8_t CC1101_AGCCTRL0 = 0x1D;  // AGC Control
static const uint8_t CC1101_WOREVT1 = 0x1E;   // High Byte Event0 Timeout
static const uint8_t CC1101_WOREVT0 = 0x1F;   // Low Byte Event0 Timeout
static const uint8_t CC1101_WORCTRL = 0x20;   // Wake On Radio Control
static const uint8_t CC1101_FREND1 = 0x21;    // Front End RX Configuration
static const uint8_t CC1101_FREND0 = 0x22;    // Front End TX Configuration
static const uint8_t CC1101_FSCAL3 = 0x23;    // Frequency Synthesizer Calibration
static const uint8_t CC1101_FSCAL2 = 0x24;    // Frequency Synthesizer Calibration
static const uint8_t CC1101_FSCAL1 = 0x25;    // Frequency Synthesizer Calibration
static const uint8_t CC1101_FSCAL0 = 0x26;    // Frequency Synthesizer Calibration
static const uint8_t CC1101_RCCTRL1 = 0x27;   // RC Oscillator Configuration
static const uint8_t CC1101_RCCTRL0 = 0x28;   // RC Oscillator Configuration
static const uint8_t CC1101_FSTEST = 0x29;    // Frequency Synthesizer Calibration Control
static const uint8_t CC1101_PTEST = 0x2A;     // Production Test
static const uint8_t CC1101_AGCTEST = 0x2B;   // AGC Test
static const uint8_t CC1101_TEST2 = 0x2C;     // Various Test Settings
static const uint8_t CC1101_TEST1 = 0x2D;     // Various Test Settings
static const uint8_t CC1101_TEST0 = 0x2E;     // Various Test Settings

/* Command strobes */
static const uint8_t CC1101_SRES = 0x30;  // Reset CC1101 chip
static const uint8_t CC1101_SFSTXON =
    0x31;  // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA): Go to a wait
           // state where only the synthesizer is running (for quick RX / TX turnaround).
static const uint8_t CC1101_SXOFF = 0x32;  // Turn off crystal oscillator
static const uint8_t CC1101_SCAL = 0x33;   // Calibrate frequency synthesizer and turn it off. SCAL can be strobed from
                                           // IDLE mode without setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
static const uint8_t CC1101_SRX =
    0x34;  // Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
static const uint8_t CC1101_STX = 0x35;  // In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1.
                                         // If in RX state and CCA is enabled: Only go to TX if channel is clear
static const uint8_t CC1101_SIDLE =
    0x36;  // Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
static const uint8_t CC1101_SWOR =
    0x38;  // Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if WORCTRL.RC_PD=0
static const uint8_t CC1101_SPWD = 0x39;  // Enter power down mode when CSn goes high
static const uint8_t CC1101_SFRX = 0x3A;  // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
static const uint8_t CC1101_SFTX =
    0x3B;  // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
static const uint8_t CC1101_SWORRST = 0x3C;  // Reset real time clock to Event1 value
static const uint8_t CC1101_SNOP = 0x3D;     // No operation. May be used to get access to the chip status byte

/* Status registers */
static const uint8_t CC1101_PARTNUM = 0x30;         // Chip ID
static const uint8_t CC1101_VERSION = 0x31;         // Chip ID
static const uint8_t CC1101_FREQEST = 0x32;         // Frequency Offset Estimate from Demodulator
static const uint8_t CC1101_LQI = 0x33;             // Demodulator Estimate for Link Quality
static const uint8_t CC1101_RSSI = 0x34;            // Received Signal Strength Indication
static const uint8_t CC1101_MARCSTATE = 0x35;       // Main Radio Control State Machine State
static const uint8_t CC1101_WORTIME1 = 0x36;        // High Byte of WOR Time
static const uint8_t CC1101_WORTIME0 = 0x37;        // Low Byte of WOR Time
static const uint8_t CC1101_PKTSTATUS = 0x38;       // Current GDOx Status and Packet Status
static const uint8_t CC1101_VCO_VC_DAC = 0x39;      // Current Setting from PLL Calibration Module
static const uint8_t CC1101_TXBYTES = 0x3A;         // Underflow and Number of Bytes
static const uint8_t CC1101_RXBYTES = 0x3B;         // Overflow and Number of Bytes
static const uint8_t CC1101_RCCTRL1_STATUS = 0x3C;  // Last RC Oscillator Calibration Result
static const uint8_t CC1101_RCCTRL0_STATUS = 0x3D;  // Last RC Oscillator Calibration Result

/* PATABLE & FIFO registers */
static const uint8_t CC1101_PATABLE = 0x3E;  // PATABLE address
static const uint8_t CC1101_TXFIFO = 0x3F;   // TX FIFO address
static const uint8_t CC1101_RXFIFO = 0x3F;   // RX FIFO address

/* Bit fields in the chip status byte */
// static const uint8_t CC1101_STATUS_CHIP_RD_YN_BM = 0x80;
// static const uint8_t CC1101_STATUS_STATE_BM = 0x70;
// static const uint8_t CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM = 0x0F;

/* Masks to retrieve status bit */
static const uint8_t CC1101_BITS_FIFO_UNDERFLOW = 0x80;
static const uint8_t CC1101_BITS_BYTES_IN_FIFO = 0x7F;
static const uint8_t CC1101_BITS_MARCSTATE = 0x1F;

/* Marc states */
enum CC1101MarcStates {
  CC1101_MARCSTATE_SLEEP = 0x00,
  CC1101_MARCSTATE_IDLE = 0x01,
  CC1101_MARCSTATE_XOFF = 0x02,
  CC1101_MARCSTATE_VCOON_MC = 0x03,
  CC1101_MARCSTATE_REGON_MC = 0x04,
  CC1101_MARCSTATE_MANCAL = 0x05,
  CC1101_MARCSTATE_VCOON = 0x06,
  CC1101_MARCSTATE_REGON = 0x07,
  CC1101_MARCSTATE_STARTCAL = 0x08,
  CC1101_MARCSTATE_BWBOOST = 0x09,
  CC1101_MARCSTATE_FS_LOCK = 0x0A,
  CC1101_MARCSTATE_IFADCON = 0x0B,
  CC1101_MARCSTATE_ENDCAL = 0x0C,
  CC1101_MARCSTATE_RX = 0x0D,
  CC1101_MARCSTATE_RX_END = 0x0E,
  CC1101_MARCSTATE_RX_RST = 0x0F,
  CC1101_MARCSTATE_TXRX_SWITCH = 0x10,
  CC1101_MARCSTATE_RXFIFO_OVERFLOW = 0x11,
  CC1101_MARCSTATE_FSTXON = 0x12,
  CC1101_MARCSTATE_TX = 0x13,
  CC1101_MARCSTATE_TX_END = 0x14,
  CC1101_MARCSTATE_RXTX_SWITCH = 0x15,
  CC1101_MARCSTATE_TXFIFO_UNDERFLOW = 0x16
};

static const std::vector<register_setting> SERIAL_CONFIG{
    {CC1101_FREQ2, 0x10},    {CC1101_FREQ1, 0xB0},    {CC1101_FREQ0, 0x71},    {CC1101_FSCTRL1, 0x06},
    {CC1101_MDMCFG1, 0x03},  {CC1101_MDMCFG0, 0xFF},  {CC1101_CHANNR, 0x00},   {CC1101_DEVIATN, 0x47},
    {CC1101_FREND1, 0x56},   {CC1101_MCSM0, 0x18},    {CC1101_FOCCFG, 0x16},   {CC1101_BSCFG, 0x1C},
    {CC1101_AGCCTRL2, 0xC7}, {CC1101_AGCCTRL1, 0x00}, {CC1101_AGCCTRL0, 0xB2}, {CC1101_FSCAL3, 0xE9},
    {CC1101_FSCAL2, 0x2A},   {CC1101_FSCAL1, 0x00},   {CC1101_FSCAL0, 0x1F},   {CC1101_FSTEST, 0x59},
    {CC1101_TEST2, 0x81},    {CC1101_TEST1, 0x35},    {CC1101_TEST0, 0x09},    {CC1101_PKTCTRL1, 0x04},
    {CC1101_ADDR, 0x00},     {CC1101_PKTLEN, 0x3D},   {CC1101_IOCFG2, 0x0D},   {CC1101_IOCFG0, 0x0D},
    {CC1101_PKTCTRL0, 0x32}, {CC1101_MDMCFG3, 0x93},  {CC1101_MDMCFG4, 0x07},  {CC1101_MDMCFG2, 0x30},
    {CC1101_FREND0, 0x11}};

// Somfy
//0x0D,// 00 IOCFG2   *29   *0B    GDO2 as serial output
//0x2E,// 01 IOCFG1    2E    2E    Tri-State
//0x2D,// 02 IOCFG0   *3F   *0C    GDO0 for input
//0x07,// 03 FIFOTHR   07   *47
//0xD3,// 04 SYNC1     D3    D3
//0x91,// 05 SYNC0     91    91
//0x3D,// 06 PKTLEN   *FF    3D
//0x04,// 07 PKTCTRL1  04    04
//0x32,// 08 PKTCTRL0 *45    32
//0x00,// 09 ADDR      00    00
//0x00,// 0A CHANNR    00    00
//0x06,// 0B FSCTRL1  *0F    06    152kHz IF Frquency
//0x00,// 0C FSCTRL0   00    00
//0x10,// 0D FREQ2    *1E    21    433.42 (10,AB,85: Somfy RTS Frequency)
//0xAB,// 0E FREQ1    *C4    65
//0xA5,// 0F FREQ0    *EC    e8
//0x55,// 10 MDMCFG4  *8C    55    bWidth 325kHz
//0x0A,// 11 MDMCFG3  *22   *43    Drate: 828 ((256+11)*2^5)*26000000/2^28
//0x30,// 12 MDMCFG2  *02   *B0    Modulation: ASK
//0x23,// 13 MDMCFG1  *22    23
//0xb9,// 14 MDMCFG0  *F8    b9    ChannelSpace: 350kHz
//0x00,// 15 DEVIATN  *47    00
//0x07,// 16 MCSM2     07    07
//0x00,// 17 MCSM1     30    30
//0x18,// 18 MCSM0    *04    18    Calibration: RX/TX->IDLE
//0x14,// 19 FOCCFG   *36    14
//0x6C,// 1A BSCFG     6C    6C
//0x07,// 1B AGCCTRL2 *03   *03    42 dB instead of 33dB
//0x00,// 1C AGCCTRL1 *40   *40
//0x90,// 1D AGCCTRL0 *91   *92    4dB decision boundery
//0x87,// 1E WOREVT1   87    87
//0x6B,// 1F WOREVT0   6B    6B
//0xF8,// 20 WORCTRL   F8    F8
//0x56,// 21 FREND1    56    56
//0x11,// 22 FREND0   *16    17    0x11 for no PA ramping
//0xE9,// 23 FSCAL3   *A9    E9
//0x2A,// 24 FSCAL2   *0A    2A
//0x00,// 25 FSCAL1    20    00
//0x1F,// 26 FSCAL0    0D    1F
//0x41,// 27 RCCTRL1   41    41
//0x00,// 28 RCCTRL0   00    00

static const std::vector<uint8_t> SERIAL_PATABLE = {0x00, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

};  // namespace cc1101
}  // namespace esphome
