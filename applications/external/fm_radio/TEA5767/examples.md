/**
 * Example calls for TEA5767.c library
 * 
 * Device Initialization and Readiness
 * ----------------------------------
 * uint8_t buffer[5];
 * bool is_ready = tea5767_is_device_ready();
 * 
 * Read/Write Registers
 * --------------------
 * bool read_success = tea5767_read_registers(buffer);
 * bool write_success = tea5767_write_registers(buffer);
 * 
 * Initialize the Device
 * ---------------------
 * bool init_success = tea5767_init(buffer);
 * 
 * Set Mute
 * --------
 * bool mute_success = tea5767_set_mute(buffer, true);  // To mute
 * bool unmute_success = tea5767_set_mute(buffer, false);  // To unmute
 * 
 * Set Stereo
 * ----------
 * bool stereo_on = tea5767_set_stereo(buffer, true);  // Stereo ON
 * bool stereo_off = tea5767_set_stereo(buffer, false);  // Stereo OFF
 * 
 * Seek
 * ----
 * bool seek_up_success = tea5767_seek(buffer, true);  // Seek up
 * bool seek_down_success = tea5767_seek(buffer, false);  // Seek down
 * 
 * Get/Set Frequency
 * -----------------
 * int frequency_khz;
 * bool get_freq_success = tea5767_get_frequency(buffer, &frequency_khz);
 * bool set_freq_success = tea5767_set_frequency(buffer, 101900);  // Set to 101.9 MHz
 * 
 * Get Radio Information
 * ---------------------
 * struct RADIO_INFO radio_info;
 * bool get_info_success = tea5767_get_radio_info(buffer, &radio_info);
 * 
 * Higher-Level Functions
 * ----------------------
 * tea5767_seekUp();  // Seek to next station up
 * tea5767_seekDown();  // Seek to next station down
 * tea5767_ToggleMute();  // Toggle mute state
 * tea5767_MuteOn();  // Mute the device
 * tea5767_MuteOff();  // Unmute the device
 * tea5767_SetFreqKHz(101900);  // Set frequency to 101.9 MHz
 * tea5767_SetFreqMHz(101.9);  // Set frequency to 101.9 MHz
 * float current_freq = tea5767_GetFreq();  // Get current frequency in MHz
 * tea5767_sleep(buffer);  // Put the device to sleep
 */
