/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Parameters
***/

/*** Includes ***/
#include <storage/storage.h>

#include "common.h"

/*** Parameters ***/

/** Files and paths **/
const char* config_file = STORAGE_APP_DATA_PATH_PREFIX "/" CONFIG_FILE;
const char* smm_pfx_config_definition_file = STORAGE_APP_DATA_PATH_PREFIX
    "/" SMM_PREFIX_CONFIG_DEFINITION_FILE;
const char* dsp_files_dir = ANY_PATH("noptel_lrf_diag");

/** Submenu item names **/
const char* submenu_item_names[] = {
    "Configuration",
    "Sample",
    "Pointer ON/OFF",
    "LRF information",
    "Save LRF diagnostic",
    "Test LRX laser",
    "Test IR pointer",
    "USB serial passthrough",
    "About"};

/** Sampling mode setting parameters **/
const char* config_mode_label = "Sampling mode";
const uint8_t config_mode_values[] =
    {smm, smm | AUTO_RESTART, cmm_1hz, cmm_4hz, cmm_10hz, cmm_20hz, cmm_100hz, cmm_200hz};
const char* config_mode_names[] =
    {"SMM", "Auto SMM", "1 Hz", "4 Hz", "10 Hz", "20 Hz", "100 Hz", "200 Hz"};
const uint8_t nb_config_mode_values = COUNT_OF(config_mode_values);

/** Buffering setting parameters **/
const char* config_buf_label = "Buffering";
const int16_t config_buf_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -5, -10, -100, -1000};
const char* config_buf_names[] = {
    "None",
    "1 s",
    "2 s",
    "3 s",
    "4 s",
    "5 s",
    "6 s",
    "7 s",
    "8 s",
    "9 s",
    "10 s",
    "5 spl",
    "10 spl",
    "100 spl",
    "1000 spl"};
const uint8_t nb_config_buf_values = COUNT_OF(config_buf_values);

/** Beep setting parameters **/
const char* config_beep_label = "Beep";
const uint8_t config_beep_values[] = {0, 1};
const char* config_beep_names[] = {"Off", "On"};
const uint8_t nb_config_beep_values = COUNT_OF(config_beep_values);

/** Baudrate setting parameters **/
const char* config_baudrate_label = "Baudrate";
const uint32_t config_baudrate_values[] = {115200, 57600, 38400, 19200, 9600};
const char* config_baudrate_names[] = {"115200", "57600", "38400", "19200", "9600"};
const uint8_t nb_config_baudrate_values = COUNT_OF(config_baudrate_values);

/** USB passthrough channel setting parameters **/
const char* config_passthru_chan_label = "Passthru channel";
const uint8_t config_passthru_chan_values[] = {0, 1};
const char* config_passthru_chan_names[] = {"0", "1"};
const uint8_t nb_config_passthru_chan_values = COUNT_OF(config_passthru_chan_values);

/** Partial SMM prefix setting parameters (the rest is in the .def file) **/
const uint8_t config_smm_pfx_values[] = {0, 1};
const uint8_t nb_config_smm_pfx_values = COUNT_OF(config_smm_pfx_values);

/** UART receive timeout **/
const uint16_t uart_rx_timeout = 500; /*ms*/

/** Speaker parameters **/
const uint16_t beep_frequency = 1000; /*Hz*/
const uint16_t sample_received_beep_duration = 25; /*ms*/

/** LED parameters **/
const uint16_t min_led_flash_duration = 15; /*ms*/

/** Sample view timings **/
const uint16_t sample_view_update_every = 150; /*ms*/
const uint8_t sample_view_smm_prefix_enabled_blink_every = 3; /*view updates*/

/** Test laser view timings **/
const uint16_t test_laser_view_update_every = 150; /*ms*/
const uint16_t test_laser_restart_cmm_every = 500; /*ms*/

/** Test pointer view timings **/
const uint16_t test_pointer_view_update_every = 150; /*ms*/
const uint16_t test_pointer_jiggle_every = 60; /*ms*/

/** USB serial passthrough view timings **/
const uint16_t passthru_view_update_every = 250; /*ms*/
