## Example calls for TEA5767.c library

Device Initialization and Readiness
----------------------------------
uint8_t buffer[5];
bool is_ready = tea5767_is_device_ready();

Read/Write Registers
--------------------
bool read_success = tea5767_read_registers(buffer);
bool write_success = tea5767_write_registers(buffer);

Initialize the Device
---------------------
bool init_success = tea5767_init(buffer);

Set Mute
--------
bool mute_success = tea5767_set_mute(buffer, true);  // To mute
bool unmute_success = tea5767_set_mute(buffer, false);  // To unmute

Set Stereo
----------
bool stereo_on = tea5767_set_stereo(buffer, true);  // Stereo ON
bool stereo_off = tea5767_set_stereo(buffer, false);  // Stereo OFF

Seek
----
bool seek_up_success = tea5767_seek(buffer, true);  // Seek up
bool seek_down_success = tea5767_seek(buffer, false);  // Seek down

Get/Set Frequency
-----------------
int frequency_khz;
bool get_freq_success = tea5767_get_frequency(buffer, &frequency_khz);
bool set_freq_success = tea5767_set_frequency(buffer, 101900);  // Set to 101.9 MHz

Get Radio Information
---------------------
struct RADIO_INFO radio_info;
bool get_info_success = tea5767_get_radio_info(buffer, &radio_info);

Higher-Level Functions
----------------------
tea5767_seekUp();  // Seek to next station up
tea5767_seekDown();  // Seek to next station down
tea5767_ToggleMute();  // Toggle mute state
tea5767_MuteOn();  // Mute the device
tea5767_MuteOff();  // Unmute the device
tea5767_SetFreqKHz(101900);  // Set frequency to 101.9 MHz
tea5767_SetFreqMHz(101.9);  // Set frequency to 101.9 MHz
float current_freq = tea5767_GetFreq();  // Get current frequency in MHz
tea5767_sleep(buffer);  // Put the device to sleep


## i2C Registers for TEA5767:
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
