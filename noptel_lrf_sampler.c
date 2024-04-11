/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.1
 *
 * Main app
***/

/*** Includes ***/
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <storage/storage.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "noptel_lrf_sampler_icons.h"	/* Generated from images in assets */
#include "lrf_serial_comm.h"



/*** Defines ***/
#define VERSION "1.1"
#define TAG "noptel_lrf_sampler"
#define SAMPLES_RING_BUFFER_SIZE 2500	/* Should hold at least 10 seconds worth
					   of samples at 200 Hz */
#define NO_AVERAGE -1		/* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2	/* This distance will be displayed as a blank */



/*** Parameters ***/
static const char *config_file = STORAGE_APP_DATA_PATH_PREFIX
				"/noptel_lrf_sampler.save";

static const char *config_freq_label = "Frequency";
static int16_t config_freq_values[] = {smm, cmm_1hz, cmm_4hz, cmm_10hz,
					cmm_20hz, cmm_100hz, cmm_200hz};
static char *config_freq_names[] = {"SMM", "1 Hz", "4 Hz", "10 Hz",
					"20 Hz", "100 Hz", "200 Hz"};

static const char *config_avg_label = "Averaging time";
static uint8_t config_avg_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static char *config_avg_names[] = {"None", "1 s", "2 s", "3 s",
					"4 s", "5 s", "6 s", "7 s",
					"8 s", "9 s", "10 s"};

static const char *config_beep_label = "Beep";
static uint8_t config_beep_values[] = {0, 1};
static char *config_beep_names[] = {"Off", "On"};

static uint16_t min_beep_duration = 70; /*ms*/

static uint16_t sample_view_update_every = 150; /*ms*/



/*** Types */

/** Submenu items **/
typedef enum {

  /* Configuration view */
  submenu_config = 0,

  /* Sample view */
  submenu_sample = 1,

  /* LRF info view */
  submenu_lrfinfo = 2,

  /* About view */
  submenu_about = 3,

  /* Total number of items */
  total_submenu_items = 4,

} SubmenuIndex;



/** Views **/
typedef enum {

  /* Submenu */
  view_submenu,

  /* Configuration view */
  view_config,

  /* Sample view */
  view_sample,

  /* LRF info view */
  view_lrfinfo,

  /* About view */
  view_about,

} AppView;



/** Configuration values **/
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

  /* Configuration values */
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

  /* Flag to play a beep, and whether a beep is already playing */
  bool play_beep;
  bool beep_playing;

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

  /* App notifications (used for backlight control) */
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

  /* Timer to control the speaker */
  FuriTimer *speaker_control_timer;

  /* LRF serial communication app */
  LRFSerialCommApp *lrf_serial_comm_app;

} App;



/*** Routines ***/

/** Time difference in seconds between system ticks in milliseconds, taking the
timestamp overflow into account **/
static double ms_tick_time_diff(uint32_t tstamp1, uint32_t tstamp2) {

  double timediff_s;

  if(tstamp1 >= tstamp2)
    timediff_s = (tstamp1 - tstamp2) / 1000.0;

  else
    timediff_s = (0xffffffff - tstamp2 + 1 + tstamp1) / 1000.0;

  return(timediff_s);
}



/** Turn the LRF on or off
    Control the LRF using the C1 pin **/
static void power_lrf(bool on) {

  /* Should we turn the LRF on? */
  if(on) {

    /* Set pin C1 to output with push-pull resistors */
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);

    /* Set the pin to true so it outputs 3.3V */
    furi_hal_gpio_write(&gpio_ext_pc1, true);

    /* Wait a bit to let the LRF boot up and be ready to accept commands */
    furi_delay_ms(300);
  }

  else {

    /* Set the C1 pin to false so it outputs 0V */
    furi_hal_gpio_write(&gpio_ext_pc1, false);

    /* Reset the pin to its default state */
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeAnalog);
  }

}



/** Load saved configuration options
    Silently fail **/
static void load_configuration(App *app) {

  SamplerModel *sampler_model = view_get_model(app->sample_view);
  Storage* storage;
  File* file;
  Config read_config;
  bool file_read = false;
  uint16_t bytes_read = 0;
  uint8_t freq_idx, avg_idx, beep_idx;

  /* Open storage and allocate space for the file*/
  storage = furi_record_open(RECORD_STORAGE);
  file = storage_file_alloc(storage);

  /* Attempt to open the file */
  if(storage_file_open(file, config_file, FSAM_READ, FSOM_OPEN_EXISTING)) {

    /* Read the file */
    bytes_read = storage_file_read(file, &read_config, sizeof(Config));

    /* Close the file */
    storage_file_close(file);

    file_read = true;
  }
  else
    FURI_LOG_I(TAG, "Could not read config file %s", config_file);

  /* Free the file and close storage */
  storage_file_free(file);
  furi_record_close(RECORD_STORAGE);

  /* If we couldn't read the file, give up */
  if(!file_read)
    return;

  /* If we didn't read enough bytes, give up */
  if(bytes_read < sizeof(Config)) {
    FURI_LOG_I(TAG, "Read %d bytes from config file %s but %d expected",
			bytes_read, config_file, sizeof(Config));
    return;
  }

  /* Check that the frequency exists */
  for(freq_idx = 0; freq_idx < COUNT_OF(config_freq_values) &&
			read_config.freq != config_freq_values[freq_idx];
	freq_idx++);

  if(freq_idx >= COUNT_OF(config_freq_values)) {
    FURI_LOG_I(TAG, "Invalid frequency value %d in config file %s",
			read_config.freq, config_file);
    return;
  }

  /* Check that the averaging time exists */
  for(avg_idx = 0; avg_idx < COUNT_OF(config_avg_values) &&
			read_config.avg != config_avg_values[avg_idx];
	avg_idx++);

  if(avg_idx >= COUNT_OF(config_avg_values)) {
    FURI_LOG_I(TAG, "Invalid averaging time value %d in config file %s",
			read_config.avg, config_file);
    return;
  }

  /* Check that the beep option exists */
  for(beep_idx = 0; beep_idx < COUNT_OF(config_beep_values) &&
			read_config.beep != config_beep_values[beep_idx];
	beep_idx++);

  if(beep_idx >= COUNT_OF(config_beep_values)) {
    FURI_LOG_I(TAG, "Invalid beep option %d in config file %s",
			read_config.beep, config_file);
    return;
  }

  /* Check that the submenu item selection exists */
  if(read_config.sitem >= total_submenu_items) {
    FURI_LOG_I(TAG, "Invalid submenu item %d in config file %s",
			read_config.sitem, config_file);
    return;
  }

  /* Configure the frequency from the read value */
  sampler_model->config.freq = read_config.freq;
  variable_item_set_current_value_index(app->item_freq, freq_idx);
  variable_item_set_current_value_text(app->item_freq,
					config_freq_names[freq_idx]);

  /* Configure the averaging time from the read value */
  sampler_model->config.avg = read_config.avg;
  variable_item_set_current_value_index(app->item_avg, avg_idx);
  variable_item_set_current_value_text(app->item_avg,
					config_avg_names[avg_idx]);

  /* Configure the beep option from the read value */
  sampler_model->config.beep = read_config.beep;
  variable_item_set_current_value_index(app->item_beep, beep_idx);
  variable_item_set_current_value_text(app->item_beep,
					config_beep_names[beep_idx]);

  /* Restore the saved last selected submenu item */
  sampler_model->config.sitem = read_config.sitem;
  submenu_set_selected_item(app->submenu, read_config.sitem);

  FURI_LOG_I(TAG, "Restored config frequency %s, averaging time %s, beep %s, "
			"selected submenu item %d",
		config_freq_names[freq_idx], config_avg_names[avg_idx],
		config_beep_names[beep_idx], read_config.sitem);
}



/** Save configuration options
    Silently fail **/
static void save_configuration(App *app) {

  SamplerModel *sampler_model = view_get_model(app->sample_view);
  Storage* storage;
  File* file;
  bool file_written = false;
  uint16_t bytes_written = 0;

  /* Open storage and allocate space for the file*/
  storage = furi_record_open(RECORD_STORAGE);
  file = storage_file_alloc(storage);

  /* Attempt to open the file */
  if(storage_file_open(file, config_file, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {

    /* Write the file */
    bytes_written = storage_file_write(file, &sampler_model->config,
					sizeof(Config));
    /* Close the file */
    storage_file_close(file);

    file_written = true;
  }
  else
    FURI_LOG_I(TAG, "Could not open config file %s for writing", config_file);

  /* Free the file and close storage */
  storage_file_free(file);
  furi_record_close(RECORD_STORAGE);

  /* If we couldn't write the file, stop now */
  if(!file_written)
    return;

  /* If we didn't write the correct number of bytes, log the error */
  if(bytes_written != sizeof(Config))
    FURI_LOG_I(TAG, "Wrote %d bytes to config file %s but %d expected",
			bytes_written, config_file, sizeof(Config));

  FURI_LOG_I(TAG, "Config saved in file %s", config_file);
}



/** Frequency setting change function **/
static void config_freq_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);
  uint8_t idx;

  /* Get the new frequency item index */
  idx = variable_item_get_current_value_index(item);

  /* Set the new frequency */
  sampler_model->config.freq = config_freq_values[idx];
  variable_item_set_current_value_text(item, config_freq_names[idx]);

  FURI_LOG_I(TAG, "Frequency change: %s", config_freq_names[idx]);
}



/** Averaging time change function **/
static void config_avg_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);
  uint8_t idx;

  /* Get the new averaging time item index */
  idx = variable_item_get_current_value_index(item);

  /* Set the new averaging time */
  sampler_model->config.avg = config_avg_values[idx];
  variable_item_set_current_value_text(item, config_avg_names[idx]);

  FURI_LOG_I(TAG, "Averaging time change: %s", config_avg_names[idx]);
}



/** Beep option change function **/
static void config_beep_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);
  uint8_t idx;

  /* Get the new beep option item index */
  idx = variable_item_get_current_value_index(item);

  /* Set the new beep option */
  sampler_model->config.beep = config_beep_values[idx] != 0;
  variable_item_set_current_value_text(item, config_beep_names[idx]);

  FURI_LOG_I(TAG, "Beep option change: %s", config_beep_names[idx]);
}



/** Draw callback for the sample view **/
static void sample_view_draw_callback(Canvas *canvas, void *model) {

  SamplerModel *sampler_model = (SamplerModel *)model;

  /* First print all the things we need to print in the FontBigNumber font */
  canvas_set_font(canvas, FontBigNumbers);

  /* Print the measured distances if they're valid */
  if(sampler_model->disp_sample.dist1 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist1);
    canvas_draw_str(canvas, 0, 14, sampler_model->spstr);
  }

  if(sampler_model->disp_sample.dist2 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist2);
    canvas_draw_str(canvas, 0, 30, sampler_model->spstr);
  }

  if(sampler_model->disp_sample.dist3 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist3);
    canvas_draw_str(canvas, 0, 46, sampler_model->spstr);
  }

  /* If we have an effective sampling frequency, print it at the bottom */
  if(sampler_model->eff_freq >= 0) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%5.1f", sampler_model->eff_freq);
    canvas_draw_str(canvas, 0, 64, sampler_model->spstr);
  }

  /* Second print all the things we need to print in the FontPrimary font
     (bold, proportional) */
  canvas_set_font(canvas, FontPrimary);

  /* If any of the distances indicate an error or the eye safety limit was hit
    display the error in the middle of the screen */
  if(sampler_model->disp_sample.dist1 == 0.5 ||
	sampler_model->disp_sample.dist1 == 0.5 ||
	sampler_model->disp_sample.dist3 == 0.5)
  {
    canvas_draw_str(canvas, 8, 27, "ERROR / EYE SAFETY");
    canvas_draw_frame(canvas, 6, 17, 115, 12);
  }

  /* None of the distances indicate an error */
  else {

    /* Add "m" right of the distance values or indicate no samples depending
       on whether we have distances or not */
    if(sampler_model->disp_sample.dist1 > 0.5)
      canvas_draw_str(canvas, 95, 14, "m");
    else if(sampler_model->disp_sample.dist1 >= 0 &&
		sampler_model->disp_sample.dist1 < 0.5) {
      canvas_draw_str(canvas, 33, 11, "NO SAMPLE");
      canvas_draw_frame(canvas, 31, 1, 66, 12);
    }
    else if(sampler_model->disp_sample.dist1 == NO_AVERAGE) {
      canvas_draw_str(canvas, 30, 11, "NO AVERAGE");
      canvas_draw_frame(canvas, 28, 1, 73, 12);
    }

    if(sampler_model->disp_sample.dist2 > 0.5)
      canvas_draw_str(canvas, 95, 30, "m");
    else if(sampler_model->disp_sample.dist2 >= 0 &&
		sampler_model->disp_sample.dist2 < 0.5) {
      canvas_draw_str(canvas, 33, 27, "NO SAMPLE");
      canvas_draw_frame(canvas, 31, 17, 66, 12);
    }
    else if(sampler_model->disp_sample.dist2 == NO_AVERAGE) {
      canvas_draw_str(canvas, 30, 27, "NO AVERAGE");
      canvas_draw_frame(canvas, 28, 17, 73, 12);
    }

    if(sampler_model->disp_sample.dist3 > 0.5)
      canvas_draw_str(canvas, 95, 46, "m");
    else if(sampler_model->disp_sample.dist3 >= 0 &&
		sampler_model->disp_sample.dist3 < 0.5) {
      canvas_draw_str(canvas, 33, 43, "NO SAMPLE");
      canvas_draw_frame(canvas, 31, 33, 66, 12);
    }
    else if(sampler_model->disp_sample.dist3 == NO_AVERAGE) {
      canvas_draw_str(canvas, 30, 43, "NO AVERAGE");
      canvas_draw_frame(canvas, 28, 33, 73, 12);
    }
  }

  /* If we have an effective sampling frequency, print "Hz" right of
     the value */
  if(sampler_model->eff_freq >= 0)
    canvas_draw_str(canvas, 59, 64, "Hz");

  /* Print the OK button symbol followed by "Sample", "Start" or "Stop"
     in a frame at the right-hand side depending on whether we do single or
     continuous measurement, and whether continuous measurement is started */
  canvas_draw_frame(canvas, 77, 52, 51, 12);
  canvas_draw_icon(canvas, 79, 54, &I_ok_button);
  if(sampler_model->config.freq == smm)
    canvas_draw_str(canvas, 90, 62, "Sample");
  else
    if(sampler_model->cmm_started)
      canvas_draw_str(canvas, 102, 62, "Stop");
    else
      canvas_draw_str(canvas, 102, 62, "Start");

  /* Finally print all the things we need to print in the FontKeyboard font
     (thin, fixed) */
  canvas_set_font(canvas, FontKeyboard);

  /* Print amplitude values when the corresponding distances are valid */
  if(sampler_model->disp_sample.dist1 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%4d", sampler_model->disp_sample.ampl1);
    canvas_draw_str(canvas, 105, 7, sampler_model->spstr);
  }

  if(sampler_model->disp_sample.dist2 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%4d", sampler_model->disp_sample.ampl2);
    canvas_draw_str(canvas, 105, 23, sampler_model->spstr);
  }

  if(sampler_model->disp_sample.dist3 > 0.5) {
    snprintf(sampler_model->spstr,
		(volatile size_t){sizeof(sampler_model->spstr)},
		"%4d", sampler_model->disp_sample.ampl3);
    canvas_draw_str(canvas, 105, 39, sampler_model->spstr);
  }

  /* Draw a dividing line between the distances / amplitudes and the bottom
     line */
  canvas_draw_line(canvas, 0, 48, 128, 48);
}



/** Input callback for the sample view **/
static bool sample_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* If the user pressed the OK button, tell the LRF to grab a single
     measurement or start/stop continuous measurement */
  if(evt->type == InputTypePress && evt->key == InputKeyOk) {
    FURI_LOG_I(TAG, "OK button pressed");

    /* Are we doing single measurement? */
    if(sampler_model->config.freq == smm)

      /* Send a SMM command (exec mode) */
      send_lrf_command(app->lrf_serial_comm_app, smm);

    /* We're doing continuous measurement */
    else

      /* Is continuous measurement already started? */
      if(sampler_model->cmm_started) {

        /* Send a CMM-break command - 3 times to be sure */
        send_lrf_command(app->lrf_serial_comm_app, cmm_break);
        send_lrf_command(app->lrf_serial_comm_app, cmm_break);
        send_lrf_command(app->lrf_serial_comm_app, cmm_break);

        sampler_model->cmm_started = false;
      }

      /* continuous measurement isn't yet started */
      else {

        /* Send the appropriate start-CMM command (exec mode) */
        send_lrf_command(app->lrf_serial_comm_app, sampler_model->config.freq);

        sampler_model->cmm_started = true;
      }

    return true;
  }

  /* We haven't handled this event */
  return false;
}



/** Sample view update timer callback **/
static void sample_view_timer_callback(void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* Were the samples updated? */
  if(sampler_model->samples_updated) {

    /* Trigger a sample view redraw */
    with_view_model(app->sample_view, SamplerModel* _model,
			{UNUSED(_model);}, true);

    sampler_model->samples_updated = false;
  }
}



/** Sample view enter callback
    Setup the timer to update the sample view regularly **/
static void sample_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;
  uint32_t period = furi_ms_to_ticks(sample_view_update_every);

  /* Set the backlight on all the time */
  notification_message(app->notifications,
			&sequence_display_backlight_enforce_on);

  with_view_model(app->sample_view, SamplerModel* sampler_model,
	{
	  /* Initialize the displayed distances */
	  sampler_model->disp_sample.dist1 = NO_DISTANCE_DISPLAY;
	  sampler_model->disp_sample.dist2 = NO_DISTANCE_DISPLAY;
	  sampler_model->disp_sample.dist3 = NO_DISTANCE_DISPLAY;

	  /* Reset the samples ring buffer */
	  sampler_model->samples_start_i = 0;
	  sampler_model->samples_end_i = 0;
	  sampler_model->samples_updated = false;

	  /* Initialize the displayed effective sampling frequency */
	  sampler_model->eff_freq = -1;

	  /* Are we doing single measurement? */
	  if(sampler_model->config.freq == smm)

	    /* Send a SMM command (exec mode) */
	    send_lrf_command(app->lrf_serial_comm_app, smm);

	  /* We're doing continuous measurement */
	  else {

            /* Send the appropriate start-CMM command (exec mode) */
	    send_lrf_command(app->lrf_serial_comm_app,
				sampler_model->config.freq);

	    sampler_model->cmm_started = true;
	  }
	},
	false);

  /* Setup and start the view update timer */
  app->sample_view_timer = furi_timer_alloc(sample_view_timer_callback,
						FuriTimerTypePeriodic, ctx);
  furi_timer_start(app->sample_view_timer, period);
}



/** Sample view exit callback
    Stop the timer to update the sample view **/
static void sample_view_exit_callback(void *ctx) {

  App *app = (App *)ctx;

  /* Send a CMM-break command unconditionally - 3 times to be sure */
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);

  /* Set the backlight control back to auto */
  notification_message(app->notifications,
			&sequence_display_backlight_enforce_auto);

  /* Stop and free the view update timer */
  furi_timer_stop(app->sample_view_timer);
  furi_timer_free(app->sample_view_timer);
}



/** Draw callback for the LRF info view **/
static void lrfinfo_view_draw_callback(Canvas *canvas, void *model) {

  LRFInfoModel *lrfinfo_model = (LRFInfoModel *)model;

  /* First print all the things we need to print in the FontPrimary font
     (bold, proportional) */
  canvas_set_font(canvas, FontPrimary);

  /* Do we have a valid identification to display? */
  if(lrfinfo_model->has_ident) {

    /* Draw the identification fields' names */
    canvas_draw_str(canvas, 13, 8, "ID");
    canvas_draw_str(canvas, 3, 17, "Add");
    canvas_draw_str(canvas, 4, 26, "S/N");
    canvas_draw_str(canvas, 2, 35, "F/W");
    canvas_draw_str(canvas, 0, 44, "Date");

    canvas_draw_str(canvas, 90, 26, "Opt");
    canvas_draw_str(canvas, 88, 35, "Elec");
  }

  /* Print the OK button symbol followed by "Read" in a frame at the
     right-hand side */
  canvas_draw_frame(canvas, 77, 52, 51, 12);
  canvas_draw_icon(canvas, 79, 54, &I_ok_button);
  canvas_draw_str(canvas, 102, 62, "Read");

  /* Draw a dividing line between the LRF information and the bottom line */
  canvas_draw_line(canvas, 0, 48, 128, 48);

  /* Do we have a valid identification to display? */
  if(lrfinfo_model->has_ident) {

    /* Second draw the identification values in the FontSecondary font
       (normal, proportional) */
    canvas_set_font(canvas, FontSecondary);

    /* Draw the identification values */
    canvas_draw_str(canvas, 26, 8, lrfinfo_model->ident.id);
    canvas_draw_str(canvas, 26, 17, lrfinfo_model->ident.addinfo);
    canvas_draw_str(canvas, 26, 26, lrfinfo_model->ident.serial);
    canvas_draw_str(canvas, 26, 35, lrfinfo_model->ident.fwversion);
    canvas_draw_str(canvas, 26, 44, lrfinfo_model->ident.builddate);

    canvas_draw_str(canvas, 111, 26, lrfinfo_model->ident.optics);
    canvas_draw_str(canvas, 111, 35, lrfinfo_model->ident.electronics);

  }
}


/** Input callback for the LRF info view **/
static bool lrfinfo_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  LRFInfoModel *lrfinfo_model = view_get_model(app->lrfinfo_view);

  /* If the user pressed the OK button, tell the LRF to send its identification
     information */
  if(evt->type == InputTypePress && evt->key == InputKeyOk) {
    FURI_LOG_I(TAG, "OK button pressed");

    /* Invalidate the current identification - if any */
    lrfinfo_model->has_ident = false;

    /* Trigger an LRF info view redraw to clear the information currently
       displayed - if any */
    with_view_model(app->lrfinfo_view, LRFInfoModel* _model,
			{UNUSED(_model);}, true);

    /* Send a send-identification-frame command */
    send_lrf_command(app->lrf_serial_comm_app, send_ident);

    return true;
  }

  /* We haven't handled this event */
  return false;
}



/** LRF info view enter callback **/
static void lrfinfo_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;

  with_view_model(app->lrfinfo_view, LRFInfoModel* lrfinfo_model,
	{
	  /* Invalidate the current identification - if any */
	  lrfinfo_model->has_ident = false;

	  /* Send a send-identification-frame command */
	  send_lrf_command(app->lrf_serial_comm_app, send_ident);
	},
	true);
}



/** Draw callback for the about view **/
static void about_view_draw_callback(Canvas *canvas, void *model) {

  AboutModel *about_model = (AboutModel *)model;

  /* Which screen should we draw? */
  switch(about_model->screen) {

    /* Draw the splash screen with the version number */
    case 0:

      /* Draw the screen's background */
      canvas_draw_icon(canvas, 0, 0, &I_about_splash);

      /* Draw the app's version number */
      canvas_set_font(canvas, FontPrimary);
      canvas_draw_str(canvas, 1, 64, "v");
      canvas_draw_str(canvas, 6, 64, VERSION);

      /* Draw a right arrow at the top right */
      canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

      break;

    /* Draw the app purpose description screen */
    case 1:

      /* Draw the title */
      canvas_set_font(canvas, FontPrimary);
      canvas_draw_str(canvas, 15, 8, "Noptel LRF Sampler");

      /* Draw the URL */
      canvas_draw_str(canvas, 25, 62, "https://noptel.fi");

      /* Draw the description */
      canvas_set_font(canvas, FontSecondary);
      canvas_draw_str(canvas, 10, 29, "Get measurements from a");
      canvas_draw_str(canvas, 15, 40, "Noptel LRF Rangefinder");

      /* Draw a left arrow at the top left */
      canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

      /* Draw a right arrow at the top right */
      canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

      break;

    /* Draw the screen showing the GPIO pin connections */
    case 2:

      /* Draw the screen's background */
      canvas_draw_icon(canvas, 0, 0, &I_about_gpio_pin_connections);

      /* Draw the title */
      canvas_set_font(canvas, FontPrimary);
      canvas_draw_str(canvas, 11, 8, "GPIO pin connections");

      /* Draw a left arrow at the top left */
      canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

      canvas_set_font(canvas, FontSecondary);
      canvas_invert_color(canvas);

      /* Draw "LRF" over the LRF side of the pinout diagram */
      canvas_draw_str(canvas, 56, 22, "LRF");

      /* Draw "Flipper Zero" under the Flipper side of the pinout diagram */
      canvas_draw_str(canvas, 39, 62, "Flipper Zero");

      break;
  }
}


/** Input callback for the about view**/
static bool about_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  AboutModel *about_model = view_get_model(app->about_view);
  bool evt_handled = false;

  /* Was the event a button press? */
  if(evt->type == InputTypePress)

    /* Which button press was it? */
    switch(evt->key) {

      /* OK button: cycle screens */
      case InputKeyOk:
        FURI_LOG_I(TAG, "OK button pressed");
        about_model->screen = (about_model->screen + 1) % 3;
        evt_handled = true;
        break;

      /* Right button: go to the next screen */
      case InputKeyRight:
        FURI_LOG_I(TAG, "Right button pressed");
        about_model->screen = about_model->screen < 2? about_model->screen + 1 :
							about_model->screen;
        evt_handled = true;
        break;

      /* Down button: go to the next screen */
      case InputKeyDown:
        FURI_LOG_I(TAG, "Down button pressed");
        about_model->screen = about_model->screen < 2? about_model->screen + 1 :
							about_model->screen;
        evt_handled = true;
        break;

      /* Left button: go to the previous screen */
      case InputKeyLeft:
        FURI_LOG_I(TAG, "Left button pressed");
        about_model->screen = about_model->screen > 0? about_model->screen - 1 :
							about_model->screen;
        evt_handled = true;
        break;

      /* Up button: go to the previous screen */
      case InputKeyUp:
        FURI_LOG_I(TAG, "Up button pressed");
        about_model->screen = about_model->screen > 0? about_model->screen - 1 :
							about_model->screen;
        evt_handled = true;
        break;

      default:
        evt_handled = false;
  }

  /* If we haven't handled this event, return now */
  if(!evt_handled)
    return false;

  /* Trigger an about view redraw */
  with_view_model(app->about_view, AboutModel* _model, {UNUSED(_model);}, true);

  /* We handled the event */
  return true;
}



/** About view enter callback **/
static void about_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;

  with_view_model(app->about_view, AboutModel* about_model,
	{
	  /* Start at the first screen */
	  about_model->screen = 0;
	},
	true);
}



/** Speaker control timer callback **/
static void speaker_control_timer_callback(void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* Should we start beeping? */
  if(sampler_model->play_beep) {

    /* Start the speaker beeping if it wasn't beeping already */
    if(!sampler_model->beep_playing && furi_hal_speaker_acquire(500)) {
      furi_hal_speaker_start(1000, 1);
      sampler_model->beep_playing = true;
    }

    sampler_model->play_beep = false;

    /* Reschedule ourselves in one minimum beep duration */
    furi_timer_start(app->speaker_control_timer, min_beep_duration);
  }

  /* If the speaker is beeping, stop it */
  else
  {
    if(sampler_model->beep_playing && furi_hal_speaker_is_mine()) {
      furi_hal_speaker_stop();
      furi_hal_speaker_release();
      sampler_model->beep_playing = false;
    }

    /* Reschedule ourselves in 10 ms, so we can start a new beep asap */
    furi_timer_start(app->speaker_control_timer, 10);
  }
}



/** Handle submenu views switching **/
static void submenu_callback(void *ctx, uint32_t idx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  switch(idx) {

    /* Switch to the configuration view */
    case submenu_config:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_config);
      sampler_model->config.sitem = submenu_config;
      FURI_LOG_I(TAG, "Switch to configuration view");
      break;

    /* Switch to the sample view */
    case submenu_sample:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_sample);
      sampler_model->config.sitem = submenu_sample;
      FURI_LOG_I(TAG, "Switch to sample view");
      break;

    /* Switch to the LRF info view */
    case submenu_lrfinfo:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_lrfinfo);
      sampler_model->config.sitem = submenu_lrfinfo;
      FURI_LOG_I(TAG, "Switch to LRF info view");
      break;

    /* Switch to the about view */
    case submenu_about:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_about);
      sampler_model->config.sitem = submenu_about;
      FURI_LOG_I(TAG, "Switch to about view");
      break;

    default:
      break;
    }
}



/** Callback to return to the submenu **/
static uint32_t return_to_submenu_callback(void *ctx) {

  UNUSED(ctx);

  /* Switch back to the submenu */
  FURI_LOG_I(TAG, "Switch back to submenu");
  return view_submenu;
}



/* Callback to exit the submenu and the app altogether */
static uint32_t submenu_exit_callback(void *ctx) {

  UNUSED(ctx);

  /* Set the view to VIEW_NONE to exit */
  FURI_LOG_I(TAG, "Exit");
  return VIEW_NONE;
}



/** LRF sample handler
    Called when a LRF sample is available from the LRF serial
    communication app **/
void lrf_sample_handler(LRFSample *lrf_sample, void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);
  uint16_t prev_samples_end_i;
  float avg_dist1;
  float avg_dist2;
  float avg_dist3;
  uint32_t avg_ampl1;
  uint32_t avg_ampl2;
  uint32_t avg_ampl3;
  uint16_t nb_valid_samples_dist1;
  uint16_t nb_valid_samples_dist2;
  uint16_t nb_valid_samples_dist3;
  uint16_t nb_samples;
  float timediff;
  uint16_t i;

  /* If beeps are enabled and any distance in the new LRF sample is valid,
     play a beep */
  if(sampler_model->config.beep && (lrf_sample->dist1 > 0.5 ||
					lrf_sample->dist2 > 0.5 ||
					lrf_sample->dist3 > 0.5))
    sampler_model->play_beep = true;

  /* Find the next spot in the samples ring buffer */
  prev_samples_end_i = sampler_model->samples_end_i;
  i = prev_samples_end_i + 1;
  if(i >= SAMPLES_RING_BUFFER_SIZE)
    i = 0;

  /* If we have room in the ring buffer, insert the new sample */
  if(i != sampler_model->samples_start_i) {
    memcpy(&(sampler_model->samples[sampler_model->samples_end_i]),
		lrf_sample, sizeof(LRFSample));
    sampler_model->samples_end_i = i;
  }

  /* Remove samples that are too old from the ring buffer - but always try
     to keep at least 0.75 seconds worth of samples, or 2 samples, for more
     accurate effective frequency calculation, even if we don't
     average samples */
  while(sampler_model->samples_start_i != prev_samples_end_i &&
	ms_tick_time_diff(
		sampler_model->samples[prev_samples_end_i].tstamp_ms,
		sampler_model->samples[sampler_model->samples_start_i].tstamp_ms
	) > (double)(sampler_model->config.avg > 0.75?
			sampler_model->config.avg : 0.75)) {
    i = sampler_model->samples_start_i + 1;
    if(i >= SAMPLES_RING_BUFFER_SIZE)
      i = 0;
    if(i == prev_samples_end_i)
      break;
    sampler_model->samples_start_i = i;
  }

  /* Calculate the number of samples in the ring buffer */
  if(sampler_model->samples_end_i >= sampler_model->samples_start_i)
    nb_samples = sampler_model->samples_end_i - sampler_model->samples_start_i;
  else
    nb_samples = SAMPLES_RING_BUFFER_SIZE - sampler_model->samples_start_i +
			sampler_model->samples_end_i;

  /* Only one sample in the ring buffer */
  if(nb_samples == 1) {

    /* We can't calculate the effective frequency */
    sampler_model->eff_freq = -1;

    /* Display that sample directly */
    memcpy(&(sampler_model->disp_sample),
		&(sampler_model->samples[prev_samples_end_i]),
		sizeof(LRFSample));
  }

  /* We have more than one sample in the ring buffer */
  else {

    /* If we have at least 0.25 seconds between the oldest and the latest
       samples' timestamps, calculate the effective sampling frequency */
    timediff = ms_tick_time_diff(
			sampler_model->samples[prev_samples_end_i].tstamp_ms,
			sampler_model->samples[sampler_model->samples_start_i].
				tstamp_ms);
    if(timediff >= 0.25)
      sampler_model->eff_freq = (nb_samples - 1) / timediff;
    else
      sampler_model->eff_freq = - 1;

    /* If we don't average samples, display the last sample directly */
    if(sampler_model->config.avg == 0)
      memcpy(&(sampler_model->disp_sample),
		&(sampler_model->samples[prev_samples_end_i]),
		sizeof(LRFSample));

    /* ...otherwise calculate the average of the valid distances and amplitudes
       in the ring buffer and display the averages instead */
    else {
      avg_dist1 = 0;
      avg_dist2 = 0;
      avg_dist3 = 0;
      avg_ampl1 = 0;
      avg_ampl2 = 0;
      avg_ampl3 = 0;

      nb_valid_samples_dist1 = 0;
      nb_valid_samples_dist2 = 0;
      nb_valid_samples_dist3 = 0;

      nb_samples = 0;

      i = sampler_model->samples_start_i;
      while(i != sampler_model->samples_end_i) {

        if(sampler_model->samples[i].dist1 > 0.5) {
          avg_dist1 += sampler_model->samples[i].dist1;
          avg_ampl1 += sampler_model->samples[i].ampl1;
          nb_valid_samples_dist1++;
        }

        if(sampler_model->samples[i].dist2 > 0.5) {
          avg_dist2 += sampler_model->samples[i].dist2;
          avg_ampl2 += sampler_model->samples[i].ampl2;
          nb_valid_samples_dist2++;
        }

        if(sampler_model->samples[i].dist3 > 0.5) {
          avg_dist3 += sampler_model->samples[i].dist3;
          avg_ampl3 += sampler_model->samples[i].ampl3;
          nb_valid_samples_dist3++;
        }

        nb_samples++;

        i++;
        if(i >= SAMPLES_RING_BUFFER_SIZE)
          i = 0;
      }

      if(nb_valid_samples_dist1 > 0) {
        sampler_model->disp_sample.dist1 = avg_dist1 / nb_valid_samples_dist1;
        sampler_model->disp_sample.ampl1 = avg_ampl1 / nb_valid_samples_dist1;
      }
      else
        sampler_model->disp_sample.dist1 = NO_AVERAGE;

      if(nb_valid_samples_dist2 > 0) {
        sampler_model->disp_sample.dist2 = avg_dist2 / nb_valid_samples_dist2;
        sampler_model->disp_sample.ampl2 = avg_ampl2 / nb_valid_samples_dist2;
      }
      else
        sampler_model->disp_sample.dist2 = NO_AVERAGE;

      if(nb_valid_samples_dist3 > 0) {
        sampler_model->disp_sample.dist3 = avg_dist3 / nb_valid_samples_dist3;
        sampler_model->disp_sample.ampl3 = avg_ampl3 / nb_valid_samples_dist3;
      }
      else
        sampler_model->disp_sample.dist3 = NO_AVERAGE;
    }
  }

  /* Mark the samples as updated */
  sampler_model->samples_updated = true;
}



/** LRF identification handler
    Called when a LRF identification frame is available from the LRF serial
    communication app **/
void lrf_ident_handler(LRFIdent *lrf_ident, void *ctx) {

  App *app = (App *)ctx;
  LRFInfoModel *lrfinfo_model = view_get_model(app->lrfinfo_view);

  /* Copy the identification and mark it as valid */
  memcpy(&(lrfinfo_model->ident), lrf_ident, sizeof(LRFIdent));
  lrfinfo_model->has_ident = true;

  /* Trigger an LRF info view redraw */
  with_view_model(app->lrfinfo_view, LRFInfoModel* _model,
			{UNUSED(_model);}, true);
}



/** Initialize the app **/
static App *app_init() {

  FURI_LOG_I(TAG, "App init");

  /* Allocate space for the app's structure */
  App *app = (App *)malloc(sizeof(App));

  /* Open a GUI instance */
  Gui *gui = furi_record_open(RECORD_GUI);

  /* Allocate space for the view dispatcher */
  app->view_dispatcher = view_dispatcher_alloc();

  /* Configure the view dispatcher */
  view_dispatcher_enable_queue(app->view_dispatcher);
  view_dispatcher_attach_to_gui(app->view_dispatcher, gui,
				ViewDispatcherTypeFullscreen);
  view_dispatcher_set_event_callback_context(app->view_dispatcher, app);



  /* Setup the submenu */

  /* Allocate space for the submenu */
  app->submenu = submenu_alloc();

  /* Add submenu items */
  submenu_add_item(app->submenu, "Configuration",
			submenu_config, submenu_callback, app);

  submenu_add_item(app->submenu, "Sample",
			submenu_sample, submenu_callback, app);

  submenu_add_item(app->submenu, "LRF information",
			submenu_lrfinfo, submenu_callback, app);

  submenu_add_item(app->submenu, "About",
			submenu_about, submenu_callback, app);

  /* Configure the "previous" callback for the submenu, which exits the app */
  view_set_previous_callback(submenu_get_view(app->submenu),
				submenu_exit_callback);

  /* Add the submenu view */
  view_dispatcher_add_view(app->view_dispatcher, view_submenu,
				submenu_get_view(app->submenu));



  /* Setup the configuration view */

  /* Allocate space for the configuration list items */
  app->config_list = variable_item_list_alloc();
  variable_item_list_reset(app->config_list);

  /* Add configuration frequency list items */
  app->item_freq = variable_item_list_add(app->config_list,
						config_freq_label,
						COUNT_OF(config_freq_values),
						config_freq_change, app);

  /* Add configuration averaging list items */
  app->item_avg = variable_item_list_add(app->config_list,
						config_avg_label,
						COUNT_OF(config_avg_values),
						config_avg_change, app);

  /* Add beep option list items */
  app->item_beep = variable_item_list_add(app->config_list,
						config_beep_label,
						COUNT_OF(config_beep_values),
						config_beep_change, app);

  /* Configure the "previous" callback for the configuration view */
  view_set_previous_callback(variable_item_list_get_view(app->config_list),
				return_to_submenu_callback);

  /* Add the configuration view */
  view_dispatcher_add_view(app->view_dispatcher, view_config,
				variable_item_list_get_view(app->config_list));



  /* Setup the sample view */

  /* Allocate space for the sample view */
  app->sample_view = view_alloc();

  /* Setup the draw callback for the sample view */
  view_set_draw_callback(app->sample_view, sample_view_draw_callback);

  /* Setup the input callback for the sample view */
  view_set_input_callback(app->sample_view, sample_view_input_callback);

  /* Configure the "previous" callback for the sample view */
  view_set_previous_callback(app->sample_view, return_to_submenu_callback);

  /* Configure the enter and exit callbacks for the sample view */
  view_set_enter_callback(app->sample_view, sample_view_enter_callback);
  view_set_exit_callback(app->sample_view, sample_view_exit_callback);

  /* Set the context for the sample view callbacks */
  view_set_context(app->sample_view, app);

  /* Allocate the sampler model */
  view_allocate_model(app->sample_view, ViewModelTypeLockFree,
			sizeof(SamplerModel));
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* Add the sample view */
  view_dispatcher_add_view(app->view_dispatcher, view_sample, app->sample_view);



  /* Setup the LRF info view */

  /* Allocate space for the LRF info view */
  app->lrfinfo_view = view_alloc();

  /* Setup the draw callback for the LRF info view */
  view_set_draw_callback(app->lrfinfo_view, lrfinfo_view_draw_callback);

  /* Setup the input callback for the LRF info view */
  view_set_input_callback(app->lrfinfo_view, lrfinfo_view_input_callback);

  /* Configure the "previous" callback for the LRF info view */
  view_set_previous_callback(app->lrfinfo_view, return_to_submenu_callback);

  /* Configure the enter callback for the LRF info view */
  view_set_enter_callback(app->lrfinfo_view, lrfinfo_view_enter_callback);

  /* Set the context for the LRF info view callbacks */
  view_set_context(app->lrfinfo_view, app);

  /* Allocate the LRF info model */
  view_allocate_model(app->lrfinfo_view, ViewModelTypeLockFree,
			sizeof(LRFInfoModel));

  /* Add the LRF info view */
  view_dispatcher_add_view(app->view_dispatcher, view_lrfinfo,
				app->lrfinfo_view);



  /* Setup the about view */

  /* Allocate space for the sample view */
  app->about_view = view_alloc();

  /* Setup the draw callback for the about view */
  view_set_draw_callback(app->about_view, about_view_draw_callback);

  /* Setup the input callback for the about view */
  view_set_input_callback(app->about_view, about_view_input_callback);

  /* Configure the "previous" callback for the about view */
  view_set_previous_callback(app->about_view, return_to_submenu_callback);

  /* Configure the enter callback for the about view */
  view_set_enter_callback(app->about_view, about_view_enter_callback);

  /* Set the context for the about view callbacks */
  view_set_context(app->about_view, app);

  /* Allocate the about view model */
  view_allocate_model(app->about_view, ViewModelTypeLockFree,
			sizeof(AboutModel));

  /* Add the about view */
  view_dispatcher_add_view(app->view_dispatcher, view_about, app->about_view);



  /* Start out at the submenu view */
  view_dispatcher_switch_to_view(app->view_dispatcher, view_submenu);



  /* Setup the default configuration */

  /* Set the default frequency */
  sampler_model->config.freq = config_freq_values[0];
  variable_item_set_current_value_index(app->item_freq, 0);
  variable_item_set_current_value_text(app->item_freq, config_freq_names[0]);

  /* Set the default averaging time */
  sampler_model->config.avg = config_avg_values[0];
  variable_item_set_current_value_index(app->item_avg, 0);
  variable_item_set_current_value_text(app->item_avg, config_avg_names[0]);

  /* Set the default beep option */
  sampler_model->config.beep = config_beep_values[0] != 0;
  variable_item_set_current_value_index(app->item_beep, 0);
  variable_item_set_current_value_text(app->item_beep, config_beep_names[0]);

  /* Set the default submenu item */
  sampler_model->config.sitem = submenu_config;

  /* Try to load the configuration file and restore the configuration from the
     saved values */
  load_configuration(app);



  /* Setup and start the timer to control the speaker */
  sampler_model->play_beep = false;
  sampler_model->beep_playing = false;
  app->speaker_control_timer = furi_timer_alloc(speaker_control_timer_callback,
						FuriTimerTypeOnce, app);
  furi_timer_start(app->speaker_control_timer, 10);



  /* Initialize the LRF serial communication app */
  app->lrf_serial_comm_app = lrf_serial_comm_app_init();



  /* Enable notifications */
  app->notifications = furi_record_open(RECORD_NOTIFICATION);

  return app;
}



/** Free up the space allocated for the app **/
static void app_free(App *app) {

  FURI_LOG_I(TAG, "App free");

  /* Stop and free the speaker control timer */
  furi_timer_stop(app->speaker_control_timer);
  furi_timer_free(app->speaker_control_timer);

  /* Stop and free up the LRF serial communication app */
  lrf_serial_comm_app_free(app->lrf_serial_comm_app);

  /* Disable notifications */
  furi_record_close(RECORD_NOTIFICATION);

  /* Try to save the configuration */
  save_configuration(app);

  /* Remove the about view */
  view_dispatcher_remove_view(app->view_dispatcher, view_about);
  view_free(app->about_view);

  /* Remove the LRF info view */
  view_dispatcher_remove_view(app->view_dispatcher, view_lrfinfo);
  view_free(app->lrfinfo_view);

  /* Remove the sample view */
  view_dispatcher_remove_view(app->view_dispatcher, view_sample);
  view_free(app->sample_view);

  /* Remove the configuration view */
  view_dispatcher_remove_view(app->view_dispatcher, view_config);
  variable_item_list_free(app->config_list);

  /* Remove the submenu view */
  view_dispatcher_remove_view(app->view_dispatcher, view_submenu);

  /* Free the submenu */
  submenu_free(app->submenu);

  /* Free the view dispatcher */
  view_dispatcher_free(app->view_dispatcher);

  /* Close the GUI instance */
  furi_record_close(RECORD_GUI);

  /* Free the app's structure */
  free(app);
}



/** Set the log level **/
static void set_log_level() {
#ifdef FURI_DEBUG
  furi_log_set_level(FuriLogLevelTrace);
#else
  furi_log_set_level(FuriLogLevelInfo);
#endif
}



/** App entry point **/
int32_t noptel_lrf_sampler_app(void *p) {

  UNUSED(p);

  /* Set the log level */
  set_log_level();

  /* Turn on the LRF */
  FURI_LOG_I(TAG, "LRF power on");
  power_lrf(true);

  /* Initialize the app */
  App *app = app_init();

  /* Setup the callback to receive decoded LRF samples */
  set_lrf_sample_handler(app->lrf_serial_comm_app, lrf_sample_handler, app);

  /* Setup the callback to receive decoded LRF identification frames */
  set_lrf_ident_handler(app->lrf_serial_comm_app, lrf_ident_handler, app);

  /* Run the view dispatcher */
  FURI_LOG_I(TAG, "Run view dispatcher");
  view_dispatcher_run(app->view_dispatcher);

  /* Free up the space for the app */
  app_free(app);

  /* Turn off the LRF */
  FURI_LOG_I(TAG, "LRF power off");
  power_lrf(false);

  return 0;
}
