/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.6
 *
 * Main app
***/

/*** Includes ***/
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>

#include "backlight_control.h"
#include "speaker_control.h"
#include "lrf_serial_comm.h"

/*** Defines ***/
#define VERSION "1.6"
#define TAG "noptel_lrf_sampler"

#define CONFIG_FILE "noptel_lrf_sampler.save"
#define SMM_PREFIX_CONFIG_DEFINITION_FILE "smm_prefix_config.def"

#define NO_AVERAGE \
    -1 /* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2 /* This distance will be displayed as a blank */

#define AUTO_RESTART 0x80

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

/** Partial SMM prefix setting parameters (the rest is in the .def file) **/
extern const uint8_t config_smm_pfx_values[];
extern const uint8_t nb_config_smm_pfx_values;

/** Speaker parameters **/
extern const uint16_t beep_frequency;
extern const uint16_t sample_received_beep_duration;

/** Sample view timings **/
extern const uint16_t sample_view_update_every;
extern const uint8_t sample_view_smm_prefix_enabled_blink_every;

/** Test laser view timings **/
extern const uint16_t test_laser_view_update_every;
extern const uint16_t test_laser_restart_cmm_every;

/** Test pointer view timings **/
extern const uint16_t test_pointer_view_update_every;
extern const uint16_t test_pointer_jiggle_every;

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

    /* About view */
    submenu_about = 7,

    /* Total number of items */
    total_submenu_items = 8,

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

/** Sampler model **/
typedef struct {
    /* Saved configuration values */
    Config config;

    /* Scratchpad string */
    char spstr[32];

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

} SamplerModel;

/** LRF info model **/
typedef struct {
    /* Baudrate */
    uint32_t baudrate;

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
    /* Baudrate */
    uint32_t baudrate;

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

    /* Scratchpad string */
    char spstr[256];

    /* Status message */
    char status_msg1[8];
    char status_msg2[48];
    char status_msg3[48];

} SaveDiagModel;

/** Test laser model **/
typedef struct {
    /* Baudrate */
    uint32_t baudrate;

    /* Whether the IR port is busy */
    bool ir_busy;

    /* Whether IR signal was received */
    bool ir_received_prev;
    bool ir_received;

    /* Flag to indicate that CMM should be restarted */
    bool restart_cmm;

    /* Beep option */
    uint8_t beep;

} TestLaserModel;

/** Test pointer model **/
typedef struct {
    /* Baudrate */
    uint32_t baudrate;

    /* Whether the IR port is busy */
    bool ir_busy;

    /* Whether IR signal was received */
    bool ir_received_prev;
    bool ir_received;

    /* Current state of the pointer */
    bool pointer_on;

    /* Beep option */
    uint8_t beep;

} TestPointerModel;

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

} App;
