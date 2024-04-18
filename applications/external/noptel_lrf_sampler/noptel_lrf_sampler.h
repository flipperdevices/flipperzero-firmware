/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
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
#define VERSION "1.3"
#define TAG "noptel_lrf_sampler"
#define NO_AVERAGE \
    -1 /* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2 /* This distance will be displayed as a blank */

/*** Parameters ***/
extern const char* config_file;

extern const int16_t config_freq_values[];
extern const char* config_freq_names[];
extern const uint8_t nb_config_freq_values;

extern const uint8_t config_avg_values[];
extern const char* config_avg_names[];
extern const uint8_t nb_config_avg_values;

extern const uint8_t config_beep_values[];
extern const char* config_beep_names[];
extern const uint8_t nb_config_beep_values;

extern const uint16_t beep_frequency;
extern const uint16_t min_beep_duration;
extern const uint16_t sample_view_update_every;

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

    /* LRF info view */
    submenu_savediag = 4,

    /* About view */
    submenu_about = 5,

    /* Total number of items */
    total_submenu_items = 6,

} SubmenuIndex;

/** Saved configuration values **/
typedef struct {
    /* LRF frequency */
    int16_t freq;

    /* Samples averaging time */
    uint8_t avg;

    /* Beep option */
    uint8_t beep;

    /* Last selected submenu item */
    uint8_t sitem;

} Config;

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

    /* Displayed distances and amplitudes */
    LRFSample disp_sample;

    /* Displayed effective sampling frequency */
    double eff_freq;

    /* Whether continuous measurement is started */
    bool cmm_started;

    /* Flag to indicate whether the sample data was updated */
    bool samples_updated;

    /* Whether the pointer is on or off */
    bool pointer_is_on;

} SamplerModel;

/** LRF info model **/
typedef struct {
    /* LRF identification */
    LRFIdent ident;

    /* Whether we have a valid identification */
    bool has_ident;

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

    /* Scratchpad string */
    char spstr[32];

    /* Status message */
    char status_msg1[8];
    char status_msg2[48];
    char status_msg3[48];

} SaveDiagModel;

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

    /* Configuration items */
    VariableItemList* config_list;
    VariableItem* item_freq;
    VariableItem* item_avg;
    VariableItem* item_beep;

    /* Sample view */
    View* sample_view;

    /* LRF info view */
    View* lrfinfo_view;

    /* Save diagnostic view */
    View* savediag_view;

    /* About view  */
    View* about_view;

    /* Timer to update the sample view */
    FuriTimer* sample_view_timer;

    /* Backlight control */
    BacklightControl backlight_control;

    /* Speaker control */
    SpeakerControl speaker_control;

    /* LRF serial communication app */
    LRFSerialCommApp* lrf_serial_comm_app;

} App;
