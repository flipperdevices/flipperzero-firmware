/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Sample View
***/

/*** Includes ***/
#include "common.h"
#include "noptel_lrf_sampler_icons.h" /* Generated from images in assets */

/*** Routines ***/

/** Time difference in seconds between system ticks in milliseconds, taking the
    timestamp overflow into account **/
static double ms_tick_time_diff(uint32_t tstamp1, uint32_t tstamp2) {
    double timediff_s;

    if(tstamp1 >= tstamp2)
        timediff_s = (tstamp1 - tstamp2) / 1000.0;

    else
        timediff_s = (0xffffffff - tstamp2 + 1 + tstamp1) / 1000.0;

    return (timediff_s);
}

/** LRF sample handler
    Called when a LRF sample is available from the LRF serial
    communication app **/
static void lrf_sample_handler(LRFSample* lrf_sample, void* ctx) {
    App* app = (App*)ctx;
    SampleModel* sample_model = view_get_model(app->sample_view);
    uint16_t prev_samples_end_i;
    uint16_t start_i_cfg_buf_only;
    uint16_t nb_samples_cfg_buf_only;
    float avg_dist1;
    float avg_dist2;
    float avg_dist3;
    uint32_t avg_ampl1;
    uint32_t avg_ampl2;
    uint32_t avg_ampl3;
    uint16_t nb_valid_samples_dist1;
    uint16_t nb_valid_samples_dist2;
    uint16_t nb_valid_samples_dist3;
    uint16_t nb_valid_samples_any_dist;
    bool sampling_error;
    bool one_dist_valid;
    float timediff;
    uint16_t i;

    /* Determine if the rangefinder encountered an error or hit the eye safety
     limit */
    sampling_error = lrf_sample->dist1 == 0.5 || lrf_sample->dist2 == 0.5 ||
                     lrf_sample->dist3 == 0.5;

    /* A single mode measurement that didn't generate an error has turned off
     the pointer */
    if((app->config.mode & (AUTO_RESTART - 1)) == smm && !sampling_error)
        app->pointer_is_on = false;

    /* Do we do automatic single measurement? */
    if(app->config.mode == (smm | AUTO_RESTART)) {
        /* Is continuous measurement still enabled? */
        if(sample_model->continuous_meas_started) {
            /* Wait a bit for the LRF to "cool off" before triggering another
       measurement */
            furi_delay_ms(sampling_error ? 1000 : 200);

            /* Send a new SMM command if continuous measurement wasn't stopped
         during the wait */
            if(sample_model->continuous_meas_started) {
                /* Send the SMM prefix if it's enabled */
                if(app->config.smm_pfx)
                    uart_tx(
                        app->lrf_serial_comm_app,
                        app->smm_pfx_config.smm_pfx_sequence,
                        sizeof(app->smm_pfx_config.smm_pfx_sequence));

                /* Send the SMM command */
                send_lrf_command(app->lrf_serial_comm_app, smm);
            }

            /* Discard the sample if the LRF encountered an error or hit the eye
         safety limit */
            if(sampling_error) return;
        }
    }

    /* If beeps are enabled and any distance in the new LRF sample is valid,
     play a beep */
    if(app->config.beep &&
       (lrf_sample->dist1 > 0.5 || lrf_sample->dist2 > 0.5 || lrf_sample->dist3 > 0.5)) {
        start_beep(&app->speaker_control, sample_received_beep_duration);
    }

    /* Reset the ring buffer if required, or if we do single measurement */
    if(sample_model->flush_samples || app->config.mode == smm) {
        sample_model->samples_start_i = 0;
        sample_model->samples_end_i = 0;
        sample_model->nb_samples = 0;
        sample_model->flush_samples = false;
    }

    /* Find the next spot in the samples ring buffer */
    prev_samples_end_i = sample_model->samples_end_i;
    i = prev_samples_end_i + 1;
    if(i >= sample_model->max_samples) i = 0;

    /* If we have room in the ring buffer, insert the new sample */
    if(i != sample_model->samples_start_i) {
        memcpy(
            &(sample_model->samples[sample_model->samples_end_i]), lrf_sample, sizeof(LRFSample));
        sample_model->samples_end_i = i;
        sample_model->nb_samples++;
    }

    /* Do we buffer samples for a set amount of time? */
    if(app->config.buf > 0) {
        /* Remove samples that are too old but try to keep at least 0.75 seconds
       worth of samples, or 2 samples, for more accurate effective frequency
       calculation, even if we don't do buffering at all.
       Keep samples that are a slightly older than we should to avoid decimating
       samples that have come in a bit late */
        while(sample_model->samples_start_i != prev_samples_end_i &&

              (sample_model->samples_time_span = ms_tick_time_diff(
                   sample_model->samples[prev_samples_end_i].tstamp_ms,
                   sample_model->samples[sample_model->samples_start_i].tstamp_ms)) >
                  (double)(app->config.buf > 0.75 ? app->config.buf + 0.2L : 0.75)) {
            i = sample_model->samples_start_i + 1;
            if(i >= sample_model->max_samples) i = 0;

            if(i == prev_samples_end_i) break;

            sample_model->nb_samples--;
            sample_model->samples_start_i = i;
        }
    }

    /* We buffer a set number of samples */
    else {
        /* Remove samples in excess but try to keep at least 0.75 seconds worth of
       samples, or 2 samples, for more accurate effective frequency calculation,
       even if we don't do buffering at all. */
        while(sample_model->samples_start_i != prev_samples_end_i &&

              sample_model->nb_samples > -app->config.buf &&

              (sample_model->samples_time_span = ms_tick_time_diff(
                   sample_model->samples[prev_samples_end_i].tstamp_ms,
                   sample_model->samples[sample_model->samples_start_i].tstamp_ms)) > 0.75L) {
            i = sample_model->samples_start_i + 1;
            if(i >= sample_model->max_samples) i = 0;

            if(i == prev_samples_end_i) break;

            sample_model->samples_start_i = i;
            sample_model->nb_samples--;
        }
    }

    /* Only one sample in the ring buffer */
    if(sample_model->nb_samples == 1) {
        /* Display that sample directly */
        memcpy(
            &(sample_model->disp_sample),
            &(sample_model->samples[prev_samples_end_i]),
            sizeof(LRFSample));

        /* There is no time between the sample and itself */
        sample_model->samples_time_span = 0;

        /* We can't calculate the effective frequency */
        sample_model->eff_freq = -1;

        /* The return rate is 0 or 100% depending on whether the sample has any
       distance or not */
        sample_model->return_rate = (sample_model->disp_sample.dist1 > 0.5 ||
                                     sample_model->disp_sample.dist2 > 0.5 ||
                                     sample_model->disp_sample.dist3 > 0.5) ?
                                        1 :
                                        0;
    }

    /* More than one sample in the ring buffer */
    else {
        /* If we have at least 0.25 seconds between the oldest and the latest
       samples' timestamps, calculate the effective sampling frequency */
        timediff = ms_tick_time_diff(
            sample_model->samples[prev_samples_end_i].tstamp_ms,
            sample_model->samples[sample_model->samples_start_i].tstamp_ms);
        if(timediff >= 0.25) {
            sample_model->eff_freq = (sample_model->nb_samples - 1) / timediff;
            FURI_LOG_T(TAG, "Effective frequency: %lf", sample_model->eff_freq);
        } else
            sample_model->eff_freq = -1;

        /* If we don't buffer samples, display the last sample directly */
        if(app->config.buf == 0)
            memcpy(
                &(sample_model->disp_sample),
                &(sample_model->samples[prev_samples_end_i]),
                sizeof(LRFSample));

        /* We buffer samples */
        else {
            start_i_cfg_buf_only = sample_model->samples_start_i;

            /* Do we buffer samples for a set amount of time? */
            if(app->config.buf > 0) {
                /* Just to calculate averages and return rate, disregard all samples
           that are too old from the ring buffer without exceptions this time,
           but still keep samples that are slightly older than we should to
           avoid decimating samples that have come in a bit late */
                nb_samples_cfg_buf_only = sample_model->nb_samples;

                while(ms_tick_time_diff(
                          sample_model->samples[prev_samples_end_i].tstamp_ms,
                          sample_model->samples[start_i_cfg_buf_only].tstamp_ms) >
                      (double)app->config.buf + 0.2L) {
                    start_i_cfg_buf_only++;
                    if(i >= sample_model->max_samples) start_i_cfg_buf_only = 0;

                    nb_samples_cfg_buf_only--;
                }
            }

            /* We buffer a set number of samples */
            else {
                /* Just to calculate averages and return rate, disregard samples in
           excess without exceptions this time */
                nb_samples_cfg_buf_only = -app->config.buf;

                if(sample_model->nb_samples > nb_samples_cfg_buf_only) {
                    start_i_cfg_buf_only += sample_model->nb_samples - nb_samples_cfg_buf_only;
                    if(start_i_cfg_buf_only >= sample_model->max_samples)
                        start_i_cfg_buf_only -= sample_model->max_samples;
                } else
                    nb_samples_cfg_buf_only = sample_model->nb_samples;

                sample_model->samples_time_span = ms_tick_time_diff(
                    sample_model->samples[prev_samples_end_i].tstamp_ms,
                    sample_model->samples[start_i_cfg_buf_only].tstamp_ms);
            }

            /* Calculate the average of the valid distances and amplitudes in that
         subset of samples in the ring buffer corresponding strictly to the
         configured buffering setting */
            avg_dist1 = 0;
            avg_dist2 = 0;
            avg_dist3 = 0;
            avg_ampl1 = 0;
            avg_ampl2 = 0;
            avg_ampl3 = 0;

            nb_valid_samples_dist1 = 0;
            nb_valid_samples_dist2 = 0;
            nb_valid_samples_dist3 = 0;

            nb_valid_samples_any_dist = 0;

            i = start_i_cfg_buf_only;
            while(i != sample_model->samples_end_i) {
                one_dist_valid = false;

                if(sample_model->samples[i].dist1 > 0.5) {
                    avg_dist1 += sample_model->samples[i].dist1;
                    avg_ampl1 += sample_model->samples[i].ampl1;
                    one_dist_valid = true;
                    nb_valid_samples_dist1++;
                }

                if(sample_model->samples[i].dist2 > 0.5) {
                    avg_dist2 += sample_model->samples[i].dist2;
                    avg_ampl2 += sample_model->samples[i].ampl2;
                    one_dist_valid = true;
                    nb_valid_samples_dist2++;
                }

                if(sample_model->samples[i].dist3 > 0.5) {
                    avg_dist3 += sample_model->samples[i].dist3;
                    avg_ampl3 += sample_model->samples[i].ampl3;
                    one_dist_valid = true;
                    nb_valid_samples_dist3++;
                }

                if(one_dist_valid) nb_valid_samples_any_dist++;

                i++;
                if(i >= sample_model->max_samples) i = 0;
            }

            if(nb_valid_samples_dist1 > 0) {
                sample_model->disp_sample.dist1 = avg_dist1 / nb_valid_samples_dist1;
                sample_model->disp_sample.ampl1 = avg_ampl1 / nb_valid_samples_dist1;
            } else
                sample_model->disp_sample.dist1 = NO_AVERAGE;

            if(nb_valid_samples_dist2 > 0) {
                sample_model->disp_sample.dist2 = avg_dist2 / nb_valid_samples_dist2;
                sample_model->disp_sample.ampl2 = avg_ampl2 / nb_valid_samples_dist2;
            } else
                sample_model->disp_sample.dist2 = NO_AVERAGE;

            if(nb_valid_samples_dist3 > 0) {
                sample_model->disp_sample.dist3 = avg_dist3 / nb_valid_samples_dist3;
                sample_model->disp_sample.ampl3 = avg_ampl3 / nb_valid_samples_dist3;
            } else
                sample_model->disp_sample.dist3 = NO_AVERAGE;

            sample_model->return_rate =
                (double)nb_valid_samples_any_dist / nb_samples_cfg_buf_only;
        }
    }

    /* Mark the samples as updated */
    sample_model->samples_updated = true;

    /* Give the sample view update timer callback a chance to run if the LRF is
     sending samples with no delay between each samples - i.e. maxxing out the
     serial bandwidth by trying (and failing) to sample at 200 Hz and sending
     the samples at 38400 bps */
    furi_delay_ms(1);
}

/** Sample view update timer callback **/
static void sample_view_timer_callback(void* ctx) {
    App* app = (App*)ctx;
    SampleModel* sample_model = view_get_model(app->sample_view);

    /* Were the samples updated or should make the OK button symbol blink? */
    if(sample_model->samples_updated || !sample_model->symbol_blinking_ctr) {
        /* Reverse the symbol's colors and reset the blinker counter if needed  */
        if(!sample_model->symbol_blinking_ctr) {
            sample_model->symbol_reversed = !sample_model->symbol_reversed;
            sample_model->symbol_blinking_ctr = sample_view_smm_prefix_enabled_blink_every;
        }

        /* Trigger a sample view redraw */
        with_view_model(
            app->sample_view, SampleModel * _model, { UNUSED(_model); }, true);

        sample_model->samples_updated = false;
    }

    /* Count down the blinking counter if it's not disabled */
    if(sample_model->symbol_blinking_ctr >= 0) sample_model->symbol_blinking_ctr--;
}

/** Sample view enter callback
    Setup the timer to update the sample view regularly **/
void sample_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;
    uint32_t period = furi_ms_to_ticks(sample_view_update_every);

    with_view_model(
        app->sample_view,
        SampleModel * sample_model,
        {
            sample_model->config = &(app->config);

            /* Start the UART at the correct baudrate */
            start_uart(app->lrf_serial_comm_app, app->config.baudrate);

            /* Setup the callback to receive decoded LRF samples */
            set_lrf_sample_handler(app->lrf_serial_comm_app, lrf_sample_handler, app);

            /* Set the backlight on all the time */
            set_backlight(&app->backlight_control, BL_ON);

            sample_model->samples_updated = false;

            /* Don't blink the OK button symbol by default */
            sample_model->symbol_reversed = false;
            sample_model->symbol_blinking_ctr = -1;

            /* Initialize the displayed distances */
            sample_model->disp_sample.dist1 = NO_DISTANCE_DISPLAY;
            sample_model->disp_sample.dist2 = NO_DISTANCE_DISPLAY;
            sample_model->disp_sample.dist3 = NO_DISTANCE_DISPLAY;

            /* Reset the samples ring buffer and associated calculated values */
            sample_model->flush_samples = true;
            sample_model->nb_samples = 0;
            sample_model->samples_time_span = 0;
            sample_model->return_rate = 0;

            /* Initialize the displayed effective sampling frequency */
            sample_model->eff_freq = -1;

            /* Are we doing single measurement (manual or automatic)? */
            if((app->config.mode & (AUTO_RESTART - 1)) == smm) {
                /* Is the SMM prefix enabled? */
                if(app->config.smm_pfx) {
                    /* Set up the OK button symbol for blinking */
                    sample_model->symbol_blinking_ctr = 0;

                    /* Send the SMM prefix */
                    uart_tx(
                        app->lrf_serial_comm_app,
                        app->smm_pfx_config.smm_pfx_sequence,
                        sizeof(app->smm_pfx_config.smm_pfx_sequence));
                }

                /* Send the SMM command */
                send_lrf_command(app->lrf_serial_comm_app, smm);
            }

            /* Otherwise send the appropriate CMM command */
            else
                send_lrf_command(app->lrf_serial_comm_app, app->config.mode);

            /* Mark continuous measurement started as needed */
            sample_model->continuous_meas_started = app->config.mode == smm ? false : true;
        },
        false);

    /* Setup and start the view update timer */
    app->sample_view_timer =
        furi_timer_alloc(sample_view_timer_callback, FuriTimerTypePeriodic, ctx);
    furi_timer_start(app->sample_view_timer, period);
}

/** Sample view exit callback
    Stop the timer to update the sample view **/
void sample_view_exit_callback(void* ctx) {
    App* app = (App*)ctx;
    SampleModel* sample_model = view_get_model(app->sample_view);

    /* Stop continuous measurement unconditionally */
    sample_model->continuous_meas_started = false;

    /* Send a CMM-break command unconditionally - 3 times to be sure */
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    send_lrf_command(app->lrf_serial_comm_app, cmm_break);
    app->pointer_is_on = false; /* A CMM break turns the pointer off */

    /* Set the backlight back to automatic */
    set_backlight(&app->backlight_control, BL_AUTO);

    /* Unset the callback to receive decoded LRF samples */
    set_lrf_sample_handler(app->lrf_serial_comm_app, NULL, app);

    /* Stop and free the view update timer */
    furi_timer_stop(app->sample_view_timer);
    furi_timer_free(app->sample_view_timer);

    /* Stop the UART */
    stop_uart(app->lrf_serial_comm_app);
}

/** Draw callback for the sample view **/
void sample_view_draw_callback(Canvas* canvas, void* model) {
    SampleModel* sample_model = (SampleModel*)model;
    double buffer_fullness;
    uint8_t y;

    /* First print all the things we need to print in the FontBigNumber font */
    canvas_set_font(canvas, FontBigNumbers);

    /* Print the measured distances if they're valid */
    if(sample_model->disp_sample.dist1 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%8.2f",
            (double)sample_model->disp_sample.dist1);
        canvas_draw_str(canvas, 0, 14, sample_model->spstr);
    }

    if(sample_model->disp_sample.dist2 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%8.2f",
            (double)sample_model->disp_sample.dist2);
        canvas_draw_str(canvas, 0, 30, sample_model->spstr);
    }

    if(sample_model->disp_sample.dist3 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%8.2f",
            (double)sample_model->disp_sample.dist3);
        canvas_draw_str(canvas, 0, 46, sample_model->spstr);
    }

    /* If we have an effective sampling frequency, print it at the bottom */
    if(sample_model->eff_freq >= 0) {
        /* If the frequency value is below 90 Hz, display it with one decimal */
        if(sample_model->eff_freq < 90) {
            snprintf(
                sample_model->spstr, sizeof(sample_model->spstr), "%4.1f", sample_model->eff_freq);
            canvas_draw_str(canvas, 12, 64, sample_model->spstr);
        }

        /* Otherwise display it rounded to the nearest integer */
        else {
            snprintf(
                sample_model->spstr, sizeof(sample_model->spstr), "%3.0f", sample_model->eff_freq);
            canvas_draw_str(canvas, 18, 64, sample_model->spstr);
        }
    }

    /* Secondly print all the things we need to print in the FontPrimary font
     (bold, proportional) */
    canvas_set_font(canvas, FontPrimary);

    /* If any of the distances indicate an error or the eye safety limit was hit
    display the error in the middle of the screen */
    if(sample_model->disp_sample.dist1 == 0.5 || sample_model->disp_sample.dist1 == 0.5 ||
       sample_model->disp_sample.dist3 == 0.5) {
        canvas_draw_str(canvas, 8, 27, "ERROR / EYE SAFETY");
        canvas_draw_frame(canvas, 6, 17, 115, 12);
    }

    /* None of the distances indicate an error */
    else {
        /* Add "m" right of the distance values or indicate no samples depending
       on whether we have distances or not */
        if(sample_model->disp_sample.dist1 > 0.5)
            canvas_draw_str(canvas, 95, 14, "m");
        else if(sample_model->disp_sample.dist1 >= 0 && sample_model->disp_sample.dist1 < 0.5) {
            canvas_draw_str(canvas, 33, 11, "NO SAMPLE");
            canvas_draw_frame(canvas, 31, 1, 66, 12);
        } else if(sample_model->disp_sample.dist1 == NO_AVERAGE) {
            canvas_draw_str(canvas, 30, 11, "NO AVERAGE");
            canvas_draw_frame(canvas, 28, 1, 73, 12);
        }

        if(sample_model->disp_sample.dist2 > 0.5)
            canvas_draw_str(canvas, 95, 30, "m");
        else if(sample_model->disp_sample.dist2 >= 0 && sample_model->disp_sample.dist2 < 0.5) {
            canvas_draw_str(canvas, 33, 27, "NO SAMPLE");
            canvas_draw_frame(canvas, 31, 17, 66, 12);
        } else if(sample_model->disp_sample.dist2 == NO_AVERAGE) {
            canvas_draw_str(canvas, 30, 27, "NO AVERAGE");
            canvas_draw_frame(canvas, 28, 17, 73, 12);
        }

        if(sample_model->disp_sample.dist3 > 0.5)
            canvas_draw_str(canvas, 95, 46, "m");
        else if(sample_model->disp_sample.dist3 >= 0 && sample_model->disp_sample.dist3 < 0.5) {
            canvas_draw_str(canvas, 33, 43, "NO SAMPLE");
            canvas_draw_frame(canvas, 31, 33, 66, 12);
        } else if(sample_model->disp_sample.dist3 == NO_AVERAGE) {
            canvas_draw_str(canvas, 30, 43, "NO AVERAGE");
            canvas_draw_frame(canvas, 28, 33, 73, 12);
        }
    }

    /* If we have an effective sampling frequency, print "Hz" right of
     the value */
    if(sample_model->eff_freq >= 0)
        canvas_draw_str(canvas, sample_model->eff_freq < 90 ? 59 : 53, 64, "Hz");

    /* Print the OK button symbol followed by "Sample", "Start" or "Stop"
     in a frame at the right-hand side depending on whether we do single or
     continuous measurement, and whether continuous measurement is started.
     Draw the button symbol in reverse video if needed */
    canvas_draw_frame(canvas, 77, 52, 51, 12);

    if(sample_model->symbol_reversed) {
        canvas_draw_frame(canvas, 78, 53, 10, 10);
        canvas_draw_line(canvas, 88, 53, 88, 63);
        canvas_invert_color(canvas);
    }

    canvas_draw_icon(canvas, 79, 54, &I_ok_button);

    if(sample_model->symbol_reversed) canvas_invert_color(canvas);

    if(sample_model->config->mode == smm)
        canvas_draw_str(canvas, 90, 62, "Sample");
    else if(sample_model->continuous_meas_started)
        canvas_draw_str(canvas, 102, 62, "Stop");
    else
        canvas_draw_str(canvas, 102, 62, "Start");

    /* Finally print all the things we need to print in the FontKeyboard font
     (thin, fixed) */
    canvas_set_font(canvas, FontKeyboard);

    /* Print amplitude values when the corresponding distances are valid */
    if(sample_model->disp_sample.dist1 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%4d",
            sample_model->disp_sample.ampl1);
        canvas_draw_str(canvas, 105, 7, sample_model->spstr);
    }

    if(sample_model->disp_sample.dist2 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%4d",
            sample_model->disp_sample.ampl2);
        canvas_draw_str(canvas, 105, 23, sample_model->spstr);
    }

    if(sample_model->disp_sample.dist3 > 0.5) {
        snprintf(
            sample_model->spstr,
            sizeof(sample_model->spstr),
            "%4d",
            sample_model->disp_sample.ampl3);
        canvas_draw_str(canvas, 105, 39, sample_model->spstr);
    }

    /* If we do continuous measurement and we buffer samples, display how much
     of the configured buffering time or samples we hold in the ring buffer
     as a small bar at the lower left, and display the return rate as a
     second small bar at the right of it */
    if(sample_model->config->mode != smm && sample_model->config->buf != 0) {
        /* Do we buffer samples for a set amount of time? */
        if(sample_model->config->buf > 0)
            buffer_fullness = sample_model->samples_time_span / sample_model->config->buf;

        /* We buffer a set number of samples */
        else
            buffer_fullness = (double)sample_model->nb_samples / -sample_model->config->buf;

        buffer_fullness = buffer_fullness > 1.0L ? 1.0L : buffer_fullness;

        /* Display how much of the configured buffering time or samples we hold
       in the ring buffer as a bar */
        y = 63 - 14 * buffer_fullness;
        canvas_draw_line(canvas, 0, 63, 0, y);
        canvas_draw_line(canvas, 1, 63, 1, y);
        canvas_draw_line(canvas, 2, 63, 2, y);

        /* Display the return rate as a bar */
        y = 63 - 14 * sample_model->return_rate;
        canvas_draw_line(canvas, 4, 63, 4, y);
        canvas_draw_line(canvas, 5, 63, 5, y);
        canvas_draw_line(canvas, 6, 63, 6, y);
    }

    /* Draw a dividing line between the distances / amplitudes and the bottom
     line */
    canvas_draw_line(canvas, 0, 48, 128, 48);
}

/** Input callback for the sample view **/
bool sample_view_input_callback(InputEvent* evt, void* ctx) {
    App* app = (App*)ctx;
    SampleModel* sample_model = view_get_model(app->sample_view);

    /* If the user pressed the OK button, tell the LRF to grab a single
     measurement or start/stop continuous measurement */
    if(evt->type == InputTypePress && evt->key == InputKeyOk) {
        FURI_LOG_D(TAG, "OK button pressed");

        /* Are we doing single measurement (manual or automatic)? */
        if((app->config.mode & (AUTO_RESTART - 1)) == smm) {
            /* Is continuous measurement stopped? */
            if(!sample_model->continuous_meas_started) {
                /* Reset the samples ring buffer */
                sample_model->flush_samples = true;

                /* Send the SMM prefix if it's enabled */
                if(app->config.smm_pfx)
                    uart_tx(
                        app->lrf_serial_comm_app,
                        app->smm_pfx_config.smm_pfx_sequence,
                        sizeof(app->smm_pfx_config.smm_pfx_sequence));

                /* Send the SMM command */
                send_lrf_command(app->lrf_serial_comm_app, smm);
            }

            /* If we do automatic single measurement, flip the started flag */
            if(app->config.mode != smm)
                sample_model->continuous_meas_started = !sample_model->continuous_meas_started;
        }

        /* We're doing continuous measurement */
        else {
            /* Is continuous measurement already started? */
            if(sample_model->continuous_meas_started) {
                /* Send a CMM-break command - 3 times to be sure */
                send_lrf_command(app->lrf_serial_comm_app, cmm_break);
                send_lrf_command(app->lrf_serial_comm_app, cmm_break);
                send_lrf_command(app->lrf_serial_comm_app, cmm_break);
                app->pointer_is_on = false; /* A CMM break turns the pointer off */

                sample_model->continuous_meas_started = false;
            }

            /* continuous measurement isn't yet started */
            else {
                /* Reset the samples ring buffer */
                sample_model->flush_samples = true;

                /* Send the appropriate start-CMM command */
                send_lrf_command(app->lrf_serial_comm_app, app->config.mode);

                sample_model->continuous_meas_started = true;
            }
        }

        return true;
    }

    /* We haven't handled this event */
    return false;
}
