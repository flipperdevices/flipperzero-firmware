/**
 * @file TEA5767.h
 * @author Victor Micó 
 * @brief Implementation to control the TEA5767 radio chip.
 * @version 0.1
 * @date 2023-08-24
 * 
 * @copyright GPLv3
 * 
 * Ported from:
 * https://github.com/mathertel/Radio
 */

#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>

// ----- Definitions for the Wire communication

#define TEA5767_ADR 0xC0 // I2C address of TEA5767

// ----- Radio chip specific definitions including the registers

#define QUARTZ 32768
#define FILTER 225000

// Define the registers

#define REG_1 0x00
#define REG_1_MUTE 0x80
#define REG_1_SM 0x40
#define REG_1_PLL 0x3F

#define REG_2 0x01
#define REG_2_PLL 0xFF

#define REG_3 0x02
#define REG_3_MS 0x08
#define REG_3_SSL 0x60
#define REG_3_SUD 0x80
#define REG_3_STEREO 0x80

#define REG_4 0x03
#define REG_4_SMUTE 0x08
#define REG_4_XTAL 0x10
#define REG_4_BL 0x20
#define REG_4_STBY 0x40
#define REG_4_ADC 0xF0

#define REG_5 0x04
#define REG_5_PLLREF 0x80
#define REG_5_DTC 0x40

// ----- local variables
/// Band datatype.
/// The BANDs a receiver probably can implement.
enum RADIO_BAND {
    RADIO_BAND_NONE = 0, ///< No band selected.

    RADIO_BAND_FM = 0x01, ///< FM band 87.5 - 108 MHz (USA, Europe) selected.
    RADIO_BAND_FMWORLD = 0x02, ///< FM band 76 - 108 MHz (Japan, Worldwide) selected.
    RADIO_BAND_AM = 0x03, ///< AM band selected.
    RADIO_BAND_KW = 0x04, ///< KW band selected.

    RADIO_BAND_FMTX = 0x11, ///< Transmit for FM.
};

/// A structure that contains information about the radio features from the chip.
struct RADIO_INFO {
    bool active; ///< receiving is active.
    uint8_t rssi; ///< Radio Station Strength Information.
    uint8_t snr; ///< Signal Noise Ratio.
    bool rds; ///< RDS information is available.
    bool tuned; ///< A stable frequency is tuned.
    bool mono; ///< Mono mode is on.
    bool stereo; ///< Stereo audio is available
};
// store the current values of the 5 chip internal 8-bit registers
// uint8_t registers[5]; ///< registers for controlling the radio chip.
// uint8_t status[5]; ///< registers with the current status of the radio chip.

/**
 * @brief Check if device is present
 * @param i2c address of the device  
 * @return True if present, false if not.
 */
bool tea5767_is_device_ready();
bool tea5767_read_registers(uint8_t* buffer);
bool tea5767_write_registers(uint8_t* buffer);
bool tea5767_init(uint8_t* buffer);
bool tea5767_set_mute(uint8_t* buffer, bool mute);
bool tea5767_get_frequency(uint8_t* buffer, int* value);
bool tea5767_set_frequency(uint8_t* buffer, int value);
bool tea5767_get_radio_info(uint8_t* buffer, struct RADIO_INFO* info);
