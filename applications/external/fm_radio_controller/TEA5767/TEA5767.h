/**
 * @file TEA5767.h
 * @author Coolshrimp - CoolshrimpModz.com
 * @brief Header file for controlling the TEA5767 FM radio chip.
 * @version 0.1
 * @date 2023-09-29
 * 
 * 
 * TEA5767 Datasheet: https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf
 * 
 * 
 * @copyright GPLv3
 * 
 */

#ifndef TEA5767_H
#define TEA5767_H


#include <stdbool.h>
#include <stdint.h>

// Definitions for the Wire communication
#define TEA5767_ADR 0xC0 // I2C address of TEA5767
// #define TEA5767_ADR 0x60 // I2C address of TEA5767

// Radio chip specific definitions including the registers
#define QUARTZ 32768 // Frequency of the quartz crystal in Hz
#define FILTER 225000 // Frequency of the filter in Hz

// Define the registers
#define REG_1 0x00  // Register 1 address
#define REG_1_MUTE 0x80  // Mute: 1 to mute the audio output, 0 to unmute
#define REG_1_SM 0x40  // Search Mode: 1 to activate search mode, 0 for normal mode
#define REG_1_PLL 0x3F  // PLL Setting: Sets the PLL value (6 bits)

#define REG_2 0x01  // Register 2 address
#define REG_2_PLL 0xFF  // PLL Setting: Sets the PLL value (8 bits)

#define REG_3 0x02  // Register 3 address
#define REG_3_SUD 0x80  // Search Up/Down: 1 to search up, 0 to search down
#define REG_3_SSL 0x60  // Search Stop Level: Sets the level at which the search stops (2 bits)
#define REG_3_HLSI 0x10  // High/Low Side Injection: 1 for high side LO injection, 0 for low side LO injection
#define REG_3_MS 0x08  // Mono to Stereo: 1 to force mono, 0 for stereo
#define REG_3_MR 0x04  // Mute Right: 1 to mute the right audio channel and force mono, 0 to unmute
#define REG_3_ML 0x02  // Mute Left: 1 to mute the left audio channel and force mono, 0 to unmute
#define REG_3_SWP1 0x01  // Software programmable port 1: 1 for HIGH, 0 for LOW

#define REG_4 0x03  // Register 4 address
#define REG_4_SWP2 0x80  // Software programmable port 2: 1 for HIGH, 0 for LOW
#define REG_4_STBY 0x40  // Standby: 1 to activate standby mode, 0 to deactivate
#define REG_4_BL 0x20  // Band Limits: 1 for Japanese FM band, 0 for US/Europe FM band
#define REG_4_XTAL 0x10  // Clock frequency: Sets the clock frequency (see Table 16)
#define REG_4_SMUTE 0x08  // Soft Mute: 1 to activate soft mute, 0 to deactivate
#define REG_4_HCC 0x04  // High Cut Control: 1 to activate high cut control, 0 to deactivate
#define REG_4_SNC 0x02  // Stereo Noise Cancelling: 1 to activate stereo noise cancelling, 0 to deactivate
#define REG_4_SI 0x01  // Search Indicator: 1 for ready flag output on pin SWPORT1, 0 for software programmable port 1

#define REG_5 0x04  // Register 5 address
#define REG_5_PLLREF 0x80  // PLL Ref: 1 to enable the 6.5 MHz reference frequency for the PLL, 0 to disable
#define REG_5_DTC 0x40  // De-emphasis Time Constant: 1 for 75 µs, 0 for 50 µs

// ----- local variables
/// Band datatype.
/// The BANDs TEA5767 can tune to.
typedef enum {
    RADIO_BAND_FM = 0x01,       ///< FM band 87.5 - 108 MHz (USA, Europe) selected.
    RADIO_BAND_FM_JAPAN = 0x02  ///< FM band 76 - 90 MHz (Japan) selected.
} RADIO_BAND;


/// A structure that contains information about the radio features from the chip.
struct RADIO_INFO {
    float frequency; // Frequency in MHz
    int signalLevel; // Signal level
    bool stereo;     // Stereo or not
    bool muted;      // Muted or not
    char signalQuality[10];  // Field for signal quality text
};


// Function prototypes
bool tea5767_is_device_ready();
bool tea5767_read_registers(uint8_t* buffer);
bool tea5767_write_registers(uint8_t* buffer);
bool tea5767_init(uint8_t* buffer);
bool tea5767_set_mute(uint8_t* buffer, bool mute);
bool tea5767_set_stereo(uint8_t* buffer, bool stereo);
bool tea5767_seek(uint8_t* buffer, bool seek_up);
bool tea5767_get_frequency(uint8_t* buffer, int* value);
bool tea5767_set_frequency(uint8_t* buffer, int value);
bool tea5767_get_radio_info(uint8_t* buffer, struct RADIO_INFO* info);

// New high-level function prototypes
void tea5767_sleep(uint8_t* buffer);
void tea5767_seekUp();
void tea5767_seekDown();
void tea5767_ToggleMute();
void tea5767_MuteOn();
void tea5767_MuteOff();
void tea5767_SetFreqKHz(int freq_khz);
void tea5767_SetFreqMHz(float freq_mhz);
float tea5767_GetFreq();

#endif // TEA5767_H
