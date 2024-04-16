/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.2
 *
 * Sample View
***/

/*** Includes ***/
#include <notification/notification_messages.h>

#include "noptel_lrf_sampler.h"
#include "noptel_lrf_sampler_icons.h"	/* Generated from images in assets */



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



/** LRF sample handler
    Called when a LRF sample is available from the LRF serial
    communication app **/
static void lrf_sample_handler(LRFSample *lrf_sample, void *ctx) {

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
					lrf_sample->dist3 > 0.5)) {
    sampler_model->play_beep = true;
    furi_timer_start(app->speaker_control_timer, 1);
  }

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
void sample_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;
  uint32_t period = furi_ms_to_ticks(sample_view_update_every);

  /* Setup the callback to receive decoded LRF samples */
  set_lrf_sample_handler(app->lrf_serial_comm_app, lrf_sample_handler, app);

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
void sample_view_exit_callback(void *ctx) {

  App *app = (App *)ctx;

  /* Send a CMM-break command unconditionally - 3 times to be sure */
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);
  send_lrf_command(app->lrf_serial_comm_app, cmm_break);

  /* Set the backlight control back to auto */
  notification_message(app->notifications,
			&sequence_display_backlight_enforce_auto);

  /* Unset the callback to receive decoded LRF samples */
  set_lrf_sample_handler(app->lrf_serial_comm_app, NULL, app);

  /* Stop and free the view update timer */
  furi_timer_stop(app->sample_view_timer);
  furi_timer_free(app->sample_view_timer);
}



/** Draw callback for the sample view **/
void sample_view_draw_callback(Canvas *canvas, void *model) {

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
bool sample_view_input_callback(InputEvent *evt, void *ctx) {

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
