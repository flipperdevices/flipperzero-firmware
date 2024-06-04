/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Main app
***/

/*** Includes ***/
#include <furi_hal_usb_cdc.h>
#include <furi_hal_usb.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>

#include "backlight_control.h"
#include "hc11_radio_comm.h"

/*** Defines ***/
#define VERSION "0.1"
#define TAG "hc11_modem"

#define CONFIG_FILE "hc11_modem.save"

#define NB_HEX_VALS_IN_PASSTHRU_SCREEN \
    90 /* 7 lines of 13 hex values,
						   minus 1 for the left arrow */

/*** Parameters ***/

/** Files and paths **/
extern const char* config_file;
extern const char* smm_pfx_config_definition_file;
extern const char* dsp_files_dir;

/** Submenu item names **/
extern const char* submenu_item_names[];

/** Channel setting parameters **/
extern const char* config_channel_label;
extern const uint8_t config_channel_values[];
extern const char* config_channel_names[];
extern const uint8_t nb_config_channel_values;

/** Address setting parameters **/
extern const char* config_address_label;
extern const uint8_t config_address_values[];
extern const char* config_address_names[];
extern const uint8_t nb_config_address_values;

/** TX power setting parameters **/
extern const char* config_txpower_label;
extern const uint8_t config_txpower_values[];
extern const char* config_txpower_names[];
extern const uint8_t nb_config_txpower_values;

/** HC-11 transmission mode setting parameters **/
extern const char* config_mode_label;
extern const uint8_t config_mode_values[];
extern const char* config_mode_names[];
extern const uint8_t nb_config_mode_values;

/** USB passthrough channel setting parameters **/
extern const char* config_passthru_chan_label;
extern const uint8_t config_passthru_chan_values[];
extern const char* config_passthru_chan_names[];
extern const uint8_t nb_config_passthru_chan_values;

/** UART receive timeout **/
extern const uint16_t uart_rx_timeout;

/** LED parameters **/
extern const uint16_t min_led_flash_duration;

/** USB serial channel to use for the passthrough **/
extern const uint16_t passthru_vcp_channel;

/** USB serial passthrough view timings **/
extern const uint16_t passthru_view_update_every;

/*** Types */

/** Submenu items **/
typedef enum {

    /* Configuration view */
    submenu_config = 0,

    /* USB passthrough view */
    submenu_passthru = 1,

    /* About view */
    submenu_about = 2,

    /* Total number of items */
    total_submenu_items = 3,

} SubmenuIndex;

/** Saved configuration values **/
typedef struct {
    /* Channel option */
    uint8_t channel;

    /* Address option */
    uint8_t address;

    /* TX power option */
    uint8_t txpower;

    /* Mode option */
    uint8_t mode;

    /* USB passthrough channel option */
    uint8_t passthru_chan;

    /* Last selected submenu item */
    uint8_t sitem;

} Config;

/** Passthrough view model **/
typedef struct {
    /* Displayed screen number */
    uint8_t screen;

    /* Whether the passthrough is enabled */
    bool enabled;

    /* HC11 radio comm app */
    HC11RadioCommApp* hc11_radio_comm_app;

    /* Whether the virtual COM port is connected */
    bool vcp_connected;

    /* The state of the USB interface before reconfiguring it */
    FuriHalUsbInterface* usb_interface_state_save;

    /* Sub-GHz receive stream buffer */
    FuriStreamBuffer* rx_stream;

    /* Sub-GHz transmit buffer */
    uint8_t tx_buf[CDC_DATA_SZ];
    uint16_t tx_buf_len;

    /* Virtual COM port receive stream buffer */
    FuriStreamBuffer* vcp_rx_stream;

    /* Virtual COM port receive buffer */
    uint8_t vcp_rx_buf[CDC_DATA_SZ];
    uint16_t vcp_rx_buf_len;

    /* Virtual COM port transmit buffer */
    uint8_t vcp_tx_buf[CDC_DATA_SZ];
    uint16_t vcp_tx_buf_len;
    uint16_t vcp_last_sent;

    /* Virtual COM port RX/TX thread and its ID */
    FuriThread* vcp_rx_tx_thread;
    FuriThreadId vcp_rx_tx_thread_id;

    /* Virtual COM port send semaphore */
    FuriSemaphore* vcp_tx_sem;

    /* Total number of bytes sent */
    uint32_t total_bytes_sent;

    /* Total number of bytes received */
    uint32_t total_bytes_recv;

    /* Flag to indicate that the display needs updating, and whether any
     information about the traffic (counters or bytes) should be displayed */
    volatile bool update_display;
    bool show_serial_traffic;

    /* Time at which the display was last updated */
    uint32_t last_display_update_tstamp;

    /* Serial traffic logging prefix */
    char traffic_logging_prefix[8];

    /* Ring buffer containing the last bytes sent or received, with the MSB
     encoding whether the byte encoded in the LSB was sent or received */
    uint16_t traffic_log[NB_HEX_VALS_IN_PASSTHRU_SCREEN];
    uint8_t traffic_log_start;
    uint8_t traffic_log_len;

    /* Mutex to access the ring buffer above */
    FuriMutex* traffic_log_mutex;

    /* Copy of the ring buffer above */
    uint16_t traffic_log_copy[NB_HEX_VALS_IN_PASSTHRU_SCREEN];
    uint8_t traffic_log_start_copy;
    uint8_t traffic_log_len_copy;

    /* Scratchpad strings */
    char spstr1[16];
    char spstr2[RX_BUF_SIZE * 3 + 8];

} PassthruModel;

/** About view model **/
typedef struct {
    /* Displayed screen number */
    uint8_t screen;

} AboutModel;

/** App structure **/
typedef struct {
    /* Saved configuration values */
    Config config;

    /* View dispatcher */
    ViewDispatcher* view_dispatcher;

    /* Submenu */
    Submenu* submenu;

    /* Configuration items */
    VariableItemList* config_list;
    VariableItem* item_channel;
    VariableItem* item_address;
    VariableItem* item_txpower;
    VariableItem* item_mode;
    VariableItem* item_passthru_chan;

    /* USB serial passthrough pointer view */
    View* passthru_view;

    /* About view  */
    View* about_view;

    /* Backlight control */
    BacklightControl backlight_control;

    /* HC11 radio comm app */
    HC11RadioCommApp* hc11_radio_comm_app;

} App;
