## Example calls for TEA5767.c library<br>

Device Initialization and Readiness<br>
----------------------------------<br>
uint8_t buffer[5];<br>
bool is_ready = tea5767_is_device_ready();<br>

Read/Write Registers<br>
--------------------<br>
bool read_success = tea5767_read_registers(buffer);<br>
bool write_success = tea5767_write_registers(buffer);<br>

Initialize the Device<br>
---------------------<br>
bool init_success = tea5767_init(buffer);<br>

Set Mute<br>
--------<br>
bool mute_success = tea5767_set_mute(buffer, true);  // To mute<br>
bool unmute_success = tea5767_set_mute(buffer, false);  // To unmute<br>

Set Stereo<br>
----------<br>
bool stereo_on = tea5767_set_stereo(buffer, true);  // Stereo ON<br>
bool stereo_off = tea5767_set_stereo(buffer, false);  // Stereo OFF<br>

Seek<br>
----<br>
bool seek_up_success = tea5767_seek(buffer, true);  // Seek up<br>
bool seek_down_success = tea5767_seek(buffer, false);  // Seek down<br>

Get/Set Frequency<br>
-----------------<br>
int frequency_khz;<br>
bool get_freq_success = tea5767_get_frequency(buffer, &frequency_khz);<br>
bool set_freq_success = tea5767_set_frequency(buffer, 101900);  // Set to 101.9 MHz<br>

Get Radio Information<br>
---------------------<br>
struct RADIO_INFO radio_info;<br>
bool get_info_success = tea5767_get_radio_info(buffer, &radio_info);<br>

Higher-Level Functions<br>
----------------------<br>
tea5767_seekUp();  // Seek to next station up<br>
tea5767_seekDown();  // Seek to next station down<br>
tea5767_ToggleMute();  // Toggle mute state<br>
tea5767_MuteOn();  // Mute the device<br>
tea5767_MuteOff();  // Unmute the device<br>
tea5767_SetFreqKHz(101900);  // Set frequency to 101.9 MHz<br>
tea5767_SetFreqMHz(101.9);  // Set frequency to 101.9 MHz<br>
float current_freq = tea5767_GetFreq();  // Get current frequency in MHz<br>
tea5767_sleep(buffer);  // Put the device to sleep<br>

## i2C Registers for TEA5767:<br>
// Define the registers<br>
#define REG_1 0x00  // Register 1 address<br>
#define REG_1_MUTE 0x80  // Mute: 1 to mute the audio output, 0 to unmute<br>
#define REG_1_SM 0x40  // Search Mode: 1 to activate search mode, 0 for normal mode<br>
#define REG_1_PLL 0x3F  // PLL Setting: Sets the PLL value (6 bits)<br>

#define REG_2 0x01  // Register 2 address<br>
#define REG_2_PLL 0xFF  // PLL Setting: Sets the PLL value (8 bits)<br>

#define REG_3 0x02  // Register 3 address<br>
#define REG_3_SUD 0x80  // Search Up/Down: 1 to search up, 0 to search down<br>
#define REG_3_SSL 0x60  // Search Stop Level: Sets the level at which the search stops (2 bits)<br>
#define REG_3_HLSI 0x10  // High/Low Side Injection: 1 for high side LO injection, 0 for low side LO injection<br>
#define REG_3_MS 0x08  // Mono to Stereo: 1 to force mono, 0 for stereo<br>
#define REG_3_MR 0x04  // Mute Right: 1 to mute the right audio channel and force mono, 0 to unmute<br>
#define REG_3_ML 0x02  // Mute Left: 1 to mute the left audio channel and force mono, 0 to unmute<br>
#define REG_3_SWP1 0x01  // Software programmable port 1: 1 for HIGH, 0 for LOW<br>

#define REG_4 0x03  // Register 4 address<br>
#define REG_4_SWP2 0x80  // Software programmable port 2: 1 for HIGH, 0 for LOW<br>
#define REG_4_STBY 0x40  // Standby: 1 to activate standby mode, 0 to deactivate<br>
#define REG_4_BL 0x20  // Band Limits: 1 for Japanese FM band, 0 for US/Europe FM band<br>
#define REG_4_XTAL 0x10  // Clock frequency: Sets the clock frequency (see Table 16)<br>
#define REG_4_SMUTE 0x08  // Soft Mute: 1 to activate soft mute, 0 to deactivate<br>
#define REG_4_HCC 0x04  // High Cut Control: 1 to activate high cut control, 0 to deactivate<br>
#define REG_4_SNC 0x02  // Stereo Noise Cancelling: 1 to activate stereo noise cancelling, 0 to deactivate<br>
#define REG_4_SI 0x01  // Search Indicator: 1 for ready flag output on pin SWPORT1, 0 for software programmable port 1<br>

#define REG_5 0x04  // Register 5 address<br>
#define REG_5_PLLREF 0x80  // PLL Ref: 1 to enable the 6.5 MHz reference frequency for the PLL, 0 to disable<br>
#define REG_5_DTC 0x40  // De-emphasis Time Constant: 1 for 75 µs, 0 for 50 µs<br>
<br><br><br><br>
## Created By: Coolshrimp - https://CoolshrimpModz.com<br>
