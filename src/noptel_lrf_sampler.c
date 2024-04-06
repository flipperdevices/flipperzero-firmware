/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.0
 *
 * Main app
***/

/*** Includes ***/
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <noptel_lrf_sampler_icons.h>	/* Generated from
					   noptel_lrf_sampler.png */
#include "lrf_serial_comm.h"



/*** Defines ***/
#define TAG "noptel_lrf_sampler"
#define SAMPLES_RING_BUFFER_SIZE 2500	/* Should hold at least 10 seconds worth
					   of samples at 200 Hz */
#define NO_AVERAGE -1		/* This distance will be displayed as
				   "NO AVERAGE" */
#define NO_DISTANCE_DISPLAY -2	/* This distance will be displayed as a blank */



/*** Parameters ***/
static const char *config_freq_label = "Frequency";
static int16_t config_freq_values[] = {-1, 1, 4, 10, 20, 100, 200};
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

static uint8_t about_widget_x = 0;
static uint8_t about_widget_y = 0;
static uint8_t about_widget_w = 128;
static uint8_t about_widget_h = 64;
static char *about_text =
	"Noptel LRF sampler  \n"
	"--------------\n"
	"Get measurements from a\n"
        "Noptel LRF rangefinder\n"
	"\n"
        "https://noptel.fi/\n"
	"\n"
        "GPIO pin connections:\n"
	"---------------\n"
        "+5V (pin #1):  LRF Vdc\n"
        "GND (pin #11): LRF ground\n"
        "TX  (pin #13): LRF serial Rx\n"
        "RX  (pin #14): LRF serial Tx";



/*** Types */

/** Submenu items **/
typedef enum {

  /* Configuration view */
  submenu_config,

  /* Sample view */
  submenu_sample,

  /* "About" view */
  submenu_about,

} SubmenuIndex;



/** Views **/
typedef enum {

  /* Submenu */
  view_submenu,

  /* Configuration view */
  view_config,

  /* Sample view */
  view_sample,

  /* "About" view */
  view_about,

} AppView;



/** Sampler model **/
typedef struct {

  /* LRF frequency */
  int16_t freq;

  /* Samples averaging time */
  uint8_t avg;

  /* Beep option */
  bool beep;

  /* Sample view scratchpad string */
  char svstr[32];

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

  /* Flag to play a beep, and whether a beep is already playing */
  bool play_beep;
  bool beep_playing;

} SamplerModel;



/** App structure **/
typedef struct {

    /* App notifications (used for backlight control) */
    NotificationApp *notifications;

    /* View dispatcher */
    ViewDispatcher *view_dispatcher;

    /* Submenu */
    Submenu *submenu;

    /* Configuration  */
    VariableItemList *config_list;

    /* Sample view */
    View *sample_view;

    /* "About" widget  */
    Widget *about_widget;

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



/** Frequency setting change function **/
static void config_freq_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  uint8_t idx = variable_item_get_current_value_index(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  sampler_model->freq = config_freq_values[idx];
  variable_item_set_current_value_text(item, config_freq_names[idx]);

  FURI_LOG_I(TAG, "Frequency change: %s", config_freq_names[idx]);
}



/** Averaging time change function **/
static void config_avg_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  uint8_t idx = variable_item_get_current_value_index(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  sampler_model->avg = config_avg_values[idx];
  variable_item_set_current_value_text(item, config_avg_names[idx]);

  FURI_LOG_I(TAG, "Averaging time change: %s", config_avg_names[idx]);
}



/** Beep option change function **/
static void config_beep_change(VariableItem *item) {

  App *app = variable_item_get_context(item);
  uint8_t idx = variable_item_get_current_value_index(item);
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  sampler_model->beep = config_beep_values[idx] != 0;
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
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist1);
      canvas_draw_str(canvas, 0, 14, sampler_model->svstr);
    }

    if(sampler_model->disp_sample.dist2 > 0.5) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist2);
      canvas_draw_str(canvas, 0, 30, sampler_model->svstr);
    }

    if(sampler_model->disp_sample.dist3 > 0.5) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%8.2f", (double)sampler_model->disp_sample.dist3);
      canvas_draw_str(canvas, 0, 46, sampler_model->svstr);
    }

    /* If we have an effective sampling frequency, print it at the bottom */
    if(sampler_model->eff_freq >= 0) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%5.1f", sampler_model->eff_freq);
      canvas_draw_str(canvas, 0, 64, sampler_model->svstr);
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

    /* Print "OK=meas", "OK=start" or "OK=stop" in a frame at the right-hand
       side depending on whether we do single or continuous measurement, and
       whether continuous measurement is started */
    if(sampler_model->freq < 0) {
      canvas_draw_str(canvas, 79, 62, "OK=meas");
      canvas_draw_frame(canvas, 77, 52, 51, 12);
    }
    else
      if(sampler_model->cmm_started) {
        canvas_draw_str(canvas, 84, 62, "OK=stop");
        canvas_draw_frame(canvas, 82, 52, 46, 12);
      }
      else {
        canvas_draw_str(canvas, 82, 62, "OK=start");
        canvas_draw_frame(canvas, 80, 52, 48, 12);
      }

    /* Finally print all the things we need to print in the FontKeyboard font
       (thin, fixed) */
    canvas_set_font(canvas, FontKeyboard);

    /* Print amplitude values when the corresponding distances are valid */
    if(sampler_model->disp_sample.dist1 > 0.5) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%4d", sampler_model->disp_sample.ampl1);
      canvas_draw_str(canvas, 105, 7, sampler_model->svstr);
    }

    if(sampler_model->disp_sample.dist2 > 0.5) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%4d", sampler_model->disp_sample.ampl2);
      canvas_draw_str(canvas, 105, 23, sampler_model->svstr);
    }

    if(sampler_model->disp_sample.dist3 > 0.5) {
      snprintf(sampler_model->svstr,
		(volatile size_t){sizeof(sampler_model->svstr)},
		"%4d", sampler_model->disp_sample.ampl3);
      canvas_draw_str(canvas, 105, 39, sampler_model->svstr);
    }

    /* Draw a dividing line between the distances / amplitudes and the bottom
       line */
    canvas_draw_line(canvas, 0, 48, 128, 48);
}


/** Input callback **/
static bool sample_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);

  /* If the user pressed the OK button, tell the LRF to grab a single
     measurement or start/stop continuous measurement */
  if(evt->type == InputTypePress && evt->key == InputKeyOk) {
    FURI_LOG_I(TAG, "OK button pressed");

    /* Are we doing single measurement? */
    if(sampler_model->freq < 0)

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

        /* Send the appropriate start-CMM command */
        switch(sampler_model->freq) {

          /* Send a start-CMM command at 1Hz (exec mode) */
          case 1:
            send_lrf_command(app->lrf_serial_comm_app, cmm_1hz);
            break;

          /* Send a start-CMM command at 4Hz (exec mode) */
          case 4:
            send_lrf_command(app->lrf_serial_comm_app, cmm_4hz);
            break;

          /* Send a start-CMM command at 10Hz (exec mode) */
          case 10:
            send_lrf_command(app->lrf_serial_comm_app, cmm_10hz);
            break;

          /* Send a start-CMM command at 20Hz (exec mode) */
          case 20:
            send_lrf_command(app->lrf_serial_comm_app, cmm_20hz);
            break;

          /* Send a start-CMM command at 100Hz (exec mode) */
          case 100:
            send_lrf_command(app->lrf_serial_comm_app, cmm_100hz);
            break;

          /* Send a start-CMM command at 200Hz (exec mode) */
          case 200:
            send_lrf_command(app->lrf_serial_comm_app, cmm_200hz);
            break;
        }

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

    /* Trigger a sample view redraw: redraw the sample view by passing true to
       the last parameter of with_view_model */
    with_view_model(app->sample_view, SamplerModel* _model,
			{UNUSED(_model);}, true);
}



/** Sample view enter callback
    Setup the timer to update the sample view regularly **/
static void sample_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;
  SamplerModel *sampler_model = view_get_model(app->sample_view);
  uint32_t period = furi_ms_to_ticks(sample_view_update_every);

  /* Set the backlight on all the time */
  notification_message(app->notifications,
			&sequence_display_backlight_enforce_on);

  /* Initialize the displayed distances */
  sampler_model->disp_sample.dist1 = NO_DISTANCE_DISPLAY;
  sampler_model->disp_sample.dist2 = NO_DISTANCE_DISPLAY;
  sampler_model->disp_sample.dist3 = NO_DISTANCE_DISPLAY;

  /* Reset the samples ring buffer */
  sampler_model->samples_start_i = 0;
  sampler_model->samples_end_i = 0;

  /* Initialize the displayed effective sampling frequency */
  sampler_model->eff_freq = -1;

  /* Mark continuous measurement as not currently started */
  sampler_model->cmm_started = false;

  /* Setup and start the sample view update timer */
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

  /* Stop and free the sample view update timer */
  furi_timer_stop(app->sample_view_timer);
  furi_timer_free(app->sample_view_timer);
  app->sample_view_timer = NULL;
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

  switch(idx) {

    /* Switch to the configuration view */
    case submenu_config:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_config);
      FURI_LOG_I(TAG, "Switch to configuration view");
      break;

    /* Switch to the sample view */
    case submenu_sample:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_sample);
      FURI_LOG_I(TAG, "Switch to sample view");
      break;

    /* Switch to the "about" view */
    case submenu_about:
      view_dispatcher_switch_to_view(app->view_dispatcher, view_about);
      FURI_LOG_I(TAG, "Switch to about view");
      break;

    default:
      break;
    }
}



/** Callback to return to the submenu **/
static uint32_t navigation_submenu_callback(void *ctx) {

  UNUSED(ctx);

  /* Switch back to the submenu */
  FURI_LOG_I(TAG, "Switch back to submenu");
  return view_submenu;
}



/* Handle back button presses */
static uint32_t navigation_exit_callback(void *ctx) {

  UNUSED(ctx);

  /* Set the view to VIEW_NONE to Exit */
  FURI_LOG_I(TAG, "Exit");
  return VIEW_NONE;
}



/** LRF data handler
    Called when LRF data is available from the LRF serial communication app **/
void lrf_data_handler(LRFSample *lrf_sample, void *ctx) {

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
  if(sampler_model->beep && (lrf_sample->dist1 > 0.5 ||
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
	) > (double)(sampler_model->avg > 0.75? sampler_model->avg : 0.75)) {
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
    if(sampler_model->avg == 0)
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

  /* Add configuration submenu items */
  submenu_add_item(app->submenu, "Configuration",
			submenu_config, submenu_callback, app);

  submenu_add_item(app->submenu, "Sample",
			submenu_sample, submenu_callback, app);

  submenu_add_item(app->submenu, "About",
			submenu_about, submenu_callback, app);

  /* Configure the "previous" callback for the submenu, which exits the app */
  view_set_previous_callback(submenu_get_view(app->submenu),
				navigation_exit_callback);

  /* Add the submenu view */
  view_dispatcher_add_view(app->view_dispatcher, view_submenu,
				submenu_get_view(app->submenu));



  /* Setup the configuration view */

  /* Allocate space for the configuration list items */
  app->config_list = variable_item_list_alloc();
  variable_item_list_reset(app->config_list);

  /* Add configuration frequency list items */
  VariableItem *item_freq = variable_item_list_add(app->config_list,
						config_freq_label,
						COUNT_OF(config_freq_values),
						config_freq_change, app);

  /* Add configuration averaging list items */
  VariableItem *item_avg = variable_item_list_add(app->config_list,
						config_avg_label,
						COUNT_OF(config_avg_values),
						config_avg_change, app);

  /* Add beep option list items */
  VariableItem *item_beep = variable_item_list_add(app->config_list,
						config_beep_label,
						COUNT_OF(config_beep_values),
						config_beep_change, app);

  /* Configure the "previous" callback for the configuration view */
  view_set_previous_callback(variable_item_list_get_view(app->config_list),
				navigation_submenu_callback);

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
  view_set_previous_callback(app->sample_view, navigation_submenu_callback);

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
  view_dispatcher_add_view(app->view_dispatcher, view_sample,
				app->sample_view);



  /* Setup the "about view" */

  /* Allocate space for the "about" text widget */
  app->about_widget = widget_alloc();

  /* Create the "about" text widget */
  widget_add_text_scroll_element(app->about_widget,
					about_widget_x, about_widget_y,
					about_widget_w, about_widget_h,
					about_text);

  /* Configure the "previous" callback for the "about" view */
  view_set_previous_callback(widget_get_view(app->about_widget),
				navigation_submenu_callback);

  /* Add the "about" view */
  view_dispatcher_add_view(app->view_dispatcher, view_about,
				widget_get_view(app->about_widget));

  /* Start out at the submenu view */
  view_dispatcher_switch_to_view(app->view_dispatcher, view_submenu);



  /* Setup the default configuration */

  /* Set the default frequency */
  sampler_model->freq = config_freq_values[0];
  variable_item_set_current_value_index(item_freq, 0);
  variable_item_set_current_value_text(item_freq, config_freq_names[0]);

  /* Set the default averaging time */
  sampler_model->avg = config_avg_values[0];
  variable_item_set_current_value_index(item_avg, 0);
  variable_item_set_current_value_text(item_avg, config_avg_names[0]);

  /* Set the default beep option */
  sampler_model->beep = config_beep_values[0] != 0;
  variable_item_set_current_value_index(item_beep, 0);
  variable_item_set_current_value_text(item_beep, config_beep_names[0]);



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

  /* Remove the "about" view */
  view_dispatcher_remove_view(app->view_dispatcher, view_about);
  widget_free(app->about_widget);

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

  uint8_t lrf_power_on_attempts = 0;
  bool lrf_powered_on = false;

  /* Set the log level */
  set_log_level();

  /* Turn on the LRF if we're running on battery */
  FURI_LOG_I(TAG, "LRF power on");
  while(lrf_power_on_attempts < 5) {
    lrf_powered_on = furi_hal_power_is_otg_enabled();
    if(lrf_powered_on)
      break;
    furi_hal_power_enable_otg();
    furi_delay_ms(10);
    lrf_power_on_attempts++;
  }

  /* If the LRF was powered on, give the device some time to boot */
  if(lrf_powered_on)
    furi_delay_ms(500);

  /* Initialize the app */
  App *app = app_init();

  /* Setup the callback to receive the decoded LRF data */
  set_lrf_data_handler(app->lrf_serial_comm_app, lrf_data_handler, app);

  /* Run the view dispatcher */
  FURI_LOG_I(TAG, "Run view dispatcher");
  view_dispatcher_run(app->view_dispatcher);

  /* Free up the space for the app */
  app_free(app);

  /* Turn off the LRF if we're running on battery */
  FURI_LOG_I(TAG, "LRF power off");
  furi_hal_power_disable_otg();

  return 0;
}
