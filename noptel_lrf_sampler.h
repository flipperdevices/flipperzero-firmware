/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.2
 *
 * Main app
***/

/*** Includes ***/
#include <notification/notification.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/variable_item_list.h>

#include "speaker_control.h"
#include "lrf_serial_comm.h"



/*** Defines ***/
#define VERSION "1.2"
#define TAG "noptel_lrf_sampler"
#define SAMPLES_RING_BUFFER_SIZE 2500	/* Should hold at least 10 seconds worth
					   of samples at 200 Hz */
#define NO_AVERAGE -1		/* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2	/* This distance will be displayed as a blank */



/*** Parameters ***/
extern const char *config_file;

extern const int16_t config_freq_values[];
extern const char *config_freq_names[];
extern const uint8_t nb_config_freq_values;

extern const uint8_t config_avg_values[];
extern const char *config_avg_names[];
extern const uint8_t nb_config_avg_values;

extern const uint8_t config_beep_values[];
extern const char *config_beep_names[];
extern const uint8_t nb_config_beep_values;

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

  /* About view */
  submenu_about = 4,

  /* Total number of items */
  total_submenu_items = 5,

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
  LRFSample samples[SAMPLES_RING_BUFFER_SIZE];
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



/** About view model **/
typedef struct {

  /* Displayed screen number */
  uint8_t screen;

} AboutModel;



/** App structure **/
typedef struct {

  /* App notifications */
  NotificationApp *notifications;

  /* View dispatcher */
  ViewDispatcher *view_dispatcher;

  /* Submenu */
  Submenu *submenu;

  /* Configuration items */
  VariableItemList *config_list;
  VariableItem *item_freq;
  VariableItem *item_avg;
  VariableItem *item_beep;

  /* Sample view */
  View *sample_view;

  /* LRF info view */
  View *lrfinfo_view;

  /* About view  */
  View *about_view;

  /* Timer to update the sample view */
  FuriTimer *sample_view_timer;

  /* Speaker control */
  SpeakerControl speaker_control;

  /* LRF serial communication app */
  LRFSerialCommApp *lrf_serial_comm_app;

} App;
