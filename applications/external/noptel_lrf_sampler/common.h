/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
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
#include "speaker_control.h"
#include "lrf_serial_comm.h"

/*** Defines ***/
#define VERSION FAP_VERSION
#define TAG "noptel_lrf_sampler"

#define CONFIG_FILE "noptel_lrf_sampler.save"
#define SMM_PREFIX_CONFIG_DEFINITION_FILE "smm_prefix_config.def"

//#define USE_5V_PIN		/* As well as PC1, for power control */

#define NO_AVERAGE \
    -1 /* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2 /* This distance will be displayed as a blank */

#define AUTO_RESTART 0x80

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

/** Sampling mode setting parameters **/
extern const char* config_mode_label;
extern const uint8_t config_mode_values[];
extern const char* config_mode_names[];
extern const uint8_t nb_config_mode_values;

/** Buffering setting parameters **/
extern const char* config_buf_label;
extern const int16_t config_buf_values[];
extern const char* config_buf_names[];
extern const uint8_t nb_config_buf_values;

/** Beep setting parameters **/
extern const char* config_beep_label;
extern const uint8_t config_beep_values[];
extern const char* config_beep_names[];
extern const uint8_t nb_config_beep_values;

/** Baudrate setting parameters **/
extern const char* config_baudrate_label;
extern const uint32_t config_baudrate_values[];
extern const char* config_baudrate_names[];
extern const uint8_t nb_config_baudrate_values;

/** USB passthrough channel setting parameters **/
extern const char* config_passthru_chan_label;
extern const uint8_t config_passthru_chan_values[];
extern const char* config_passthru_chan_names[];
extern const uint8_t nb_config_passthru_chan_values;

/** Partial SMM prefix setting parameters (the rest is in the .def file) **/
extern const uint8_t config_smm_pfx_values[];
extern const uint8_t nb_config_smm_pfx_values;

/** UART receive timeout **/
extern const uint16_t uart_rx_timeout;

/** Speaker parameters **/
extern const uint16_t beep_frequency;
extern const uint16_t sample_received_beep_duration;

/** LED parameters **/
extern const uint16_t min_led_flash_duration;

/** Sample view timings **/
extern const uint16_t sample_view_update_every;
extern const uint8_t sample_view_smm_prefix_enabled_blink_every;

/** Test laser view timings **/
extern const uint16_t test_laser_view_update_every;
extern const uint16_t test_laser_restart_cmm_every;

/** Test pointer view timings **/
extern const uint16_t test_pointer_view_update_every;
extern const uint16_t test_pointer_jiggle_every;

/** USB serial channel to use for the passthrough **/
extern const uint16_t passthru_vcp_channel;

/** USB serial passthrough view timings **/
extern const uint16_t passthru_view_update_every;

/*** Types */

/** Submenu items **/
typedef enum {

    /* Configuration view */
    submenu_config = 0,

    /* Sample view */
    submenu_sample = 1,

    /* Pointer ON/OFF toggle */
    submenu_pointeronoff = 2,

    /* LRF info view */
    submenu_lrfinfo = 3,

    /* Save diagnostic view */
    submenu_savediag = 4,

    /* Test laser view */
    submenu_testlaser = 5,

    /* Test pointer view */
    submenu_testpointer = 6,

    /* USB passthrough view */
    submenu_passthru = 7,

    /* About view */
    submenu_about = 8,

    /* Total number of items */
    total_submenu_items = 9,

} SubmenuIndex;

/** Saved configuration values **/
typedef struct {
    /* LRF sampling mode setting */
    uint8_t mode;

    /* Samples buffering setting */
    int16_t buf;

    /* Beep option */
    uint8_t beep;

    /* Baudrate option */
    uint32_t baudrate;

    /* USB passthrough channel option */
    uint8_t passthru_chan;

    /* SMM prefix option */
    uint8_t smm_pfx;

    /* Last selected submenu item */
    uint8_t sitem;

} Config;

/** SMM prefix configuration option values **/
typedef struct {
    /* SMM prefix byte sequence */
    uint8_t smm_pfx_sequence[8];

    /* SMM prefix configuration option label */
    char config_smm_pfx_label[16];

    /* SMM prefix configuration choice names */
    char config_smm_pfx_names[2][16];

} SMMPfxConfig;

/** Sample view model **/
typedef struct {
    Config* config;

    /* LRF sample ring buffer */
    LRFSample* samples;
    uint16_t max_samples;
    uint16_t samples_start_i;
    uint16_t samples_end_i;

    /* Number of samples in the ring buffer */
    uint16_t nb_samples;

    /* Time difference between the oldest and newest samples in the ring buffer */
    double samples_time_span;

    /* Flag to indicate the ring buffer should be reset */
    bool flush_samples;

    /* Displayed distances and amplitudes */
    LRFSample disp_sample;

    /* Displayed effective sampling frequency */
    double eff_freq;

    /* Displayed return rate */
    double return_rate;

    /* Whether continuous measurement is started */
    bool continuous_meas_started;

    /* Flag to indicate whether the sample data was updated */
    bool samples_updated;

    /* Whether the OK button symbol should be displayed in reverse video */
    bool symbol_reversed;

    /* OK button symbol blinking counter */
    int8_t symbol_blinking_ctr;

    /* Whether the pointer is on or off */
    bool pointer_is_on;

    /* Scratchpad string */
    char spstr[32];

} SampleModel;

/** LRF info view model **/
typedef struct {
    /* LRF identification */
    LRFIdent ident;

    /* Whether we have a valid identification */
    bool has_ident;

    /* LRF information */
    LRFInfo info;

    /* Whether we have valid information */
    bool has_info;

    /* Scratchpad string */
    char spstr[8];

} LRFInfoModel;

/** Save diagnostic model **/
typedef struct {
    /* LRF identification */
    LRFIdent ident;

    /* Whether we have a valid identification */
    bool has_ident;

    /* Diagnostic data */
    LRFDiag lrf_diag;

    /* Whether diagnostic data is being downloaded */
    bool download_in_progress;

    /* Whether diagnostic data is being saved */
    bool save_in_progress;

    /* Progress (0 -> 1) */
    float progress;

    /* Diagnostic file name and path */
    char dsp_fname_pt1[16];
    char dsp_fname_pt2[32];
    char dsp_fpath[128];

    /* Status message */
    char status_msg1[8];
    char status_msg2[48];
    char status_msg3[48];

    /* Scratchpad string */
    char spstr[256];

} SaveDiagModel;

/** Test laser model **/
typedef struct {
    /* Whether the IR port is busy */
    bool ir_busy;

    /* Whether IR signal was received */
    bool ir_received_prev;
    bool ir_received;

    /* Flag to indicate that CMM should be restarted */
    bool restart_cmm;

} TestLaserModel;

/** Test pointer view model **/
typedef struct {
    /* Whether the IR port is busy */
    bool ir_busy;

    /* Whether IR signal was received */
    bool ir_received_prev;
    bool ir_received;

    /* Current state of the pointer */
    bool pointer_is_on;

} TestPointerModel;

/** Passthrough view model **/
typedef struct {
    /* Displayed screen number */
    uint8_t screen;

    /* Whether the passthrough is enabled */
    bool enabled;

    /* Whether the virtual COM port is connected */
    bool vcp_connected;

    /* The state of the USB interface before reconfiguring it */
    FuriHalUsbInterface* usb_interface_state_save;

    /* Virtual COM port configuration */
    struct usb_cdc_line_coding* vcp_config;

    /* UART baudrate and name*/
    uint32_t uart_baudrate;
    const char* uart_baudrate_name;

    /* UART receive stream buffer */
    FuriStreamBuffer* uart_rx_stream;

    /* UART transmit buffer */
    uint8_t uart_tx_buf[CDC_DATA_SZ];
    uint16_t uart_tx_buf_len;

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

    /* Total number of bytes sent to the LRF */
    uint32_t total_bytes_sent;

    /* Total number of bytes received from the LRF */
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
    char spstr2[UART_RX_BUF_SIZE * 3 + 8];

} PassthruModel;

/** About view model **/
typedef struct {
    /* Displayed screen number */
    uint8_t screen;

} AboutModel;

/** App structure **/
typedef struct {
    /* Large storage space shared between various parts of the app, because
     the Flipper Zero doesn't have enough memory for separate storage areas.
     Should be large enough to hold 2000 LRFSample samples (and then some)
     or a full LRF diagnostic frame */
    uint8_t shared_storage[60000]; /* Holds 2500 samples */

    /* Saved configuration values */
    Config config;

    /* View dispatcher */
    ViewDispatcher* view_dispatcher;

    /* Submenu */
    Submenu* submenu;

    /* SMM prefix configuration option values */
    SMMPfxConfig smm_pfx_config;

    /* Configuration items */
    VariableItemList* config_list;
    VariableItem* item_mode;
    VariableItem* item_buf;
    VariableItem* item_beep;
    VariableItem* item_baudrate;
    VariableItem* item_passthru_chan;
    VariableItem* item_smm_pfx;

    /* Sample view */
    View* sample_view;

    /* LRF info view */
    View* lrfinfo_view;

    /* Save diagnostic view */
    View* savediag_view;

    /* Test laser view */
    View* testlaser_view;

    /* Test pointer view */
    View* testpointer_view;

    /* USB serial passthrough pointer view */
    View* passthru_view;

    /* About view  */
    View* about_view;

    /* Timer to update the sample view */
    FuriTimer* sample_view_timer;

    /* Timer to update the test laser view */
    FuriTimer* test_laser_view_timer;

    /* Timer to check if CMM needs restarting in the test laser view*/
    FuriTimer* test_laser_restart_cmm_timer;

    /* Timer to update the pointer laser view */
    FuriTimer* test_pointer_view_timer;

    /* Timer to control the pointer in the test pointer view */
    FuriTimer* test_pointer_control_timer;

    /* Backlight control */
    BacklightControl backlight_control;

    /* Speaker control */
    SpeakerControl speaker_control;

    /* LRF serial communication app */
    LRFSerialCommApp* lrf_serial_comm_app;

    /* Whether the pointer is on or off */
    bool pointer_is_on;

} App;
