/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Save diagnostic view
***/

/*** Includes ***/
#include <furi_hal.h>
#include <storage/storage.h>

#include "common.h"
#include "noptel_lrf_sampler_icons.h" /* Generated from images in assets */

/*** Routines ***/

/** Time difference in milliseconds between system ticks in milliseconds,
    taking the timestamp overflow into account **/
static uint32_t ms_tick_time_diff_ms(uint32_t tstamp1, uint32_t tstamp2) {
    if(tstamp1 >= tstamp2)
        return tstamp1 - tstamp2;

    else
        return 0xffffffff - tstamp2 + 1 + tstamp1;
}

/** LRF identification handler
    Called when a LRF identification frame is available from the LRF serial
    communication app **/
static void lrf_ident_handler(LRFIdent* lrf_ident, void* ctx) {
    App* app = (App*)ctx;
    SaveDiagModel* savediag_model = view_get_model(app->savediag_view);

    /* Copy the identification and mark it as valid */
    memcpy(&(savediag_model->ident), lrf_ident, sizeof(LRFIdent));
    savediag_model->has_ident = true;
}

/** Diagnostic data handler
    Called when part or all of the diagnostic data is received by the LRF serial
    communication app **/
static void diag_data_handler(LRFDiag* lrf_diag, void* ctx) {
    App* app = (App*)ctx;
    SaveDiagModel* savediag_model = view_get_model(app->savediag_view);
    DateTime datetime;
    Storage* storage;
    File* file;
    uint32_t bytes_to_write, bytes_written;
    uint32_t total_bytes_written = 0;
    int32_t val;
    uint32_t now_ms, last_update_display;
    uint32_t i;

    /* Copy the diagnostic data */
    memcpy(&(savediag_model->lrf_diag), lrf_diag, sizeof(LRFDiag));

    savediag_model->download_in_progress = true;
    savediag_model->save_in_progress = false;

    /* Calculate the first half of the progress: downloading */
    savediag_model->progress =
        ((float)savediag_model->lrf_diag.nb_vals / (float)savediag_model->lrf_diag.total_vals) / 2;

    /* Do we have all the diagnostic data values? */
    if(savediag_model->lrf_diag.nb_vals == savediag_model->lrf_diag.total_vals) {
        savediag_model->download_in_progress = false;

        /* Do we have LRF identification data? */
        if(savediag_model->has_ident) {
            savediag_model->save_in_progress = true;

            /* Trigger a save diagnostic view redraw to bring the progress bar to
         50% before saving */
            with_view_model(
                app->savediag_view, SaveDiagModel * _model, { UNUSED(_model); }, true);
            last_update_display = furi_get_tick();

            /* Get the current date / time */
            furi_hal_rtc_get_datetime(&datetime);

            /* Create the DSP file name in two parts and absolute path to save the
         diagnostic into */
            snprintf(
                savediag_model->dsp_fname_pt1,
                sizeof(savediag_model->dsp_fname_pt1) + 1,
                "%s-",
                savediag_model->ident.serial);

            snprintf(
                savediag_model->dsp_fname_pt2,
                sizeof(savediag_model->dsp_fname_pt2),
                "%04d.%02d.%02d-%02d.%02d.%02d.dsp",
                datetime.year,
                datetime.month,
                datetime.day,
                datetime.hour,
                datetime.minute,
                datetime.second);

            snprintf(
                savediag_model->dsp_fpath,
                sizeof(savediag_model->dsp_fpath),
                "%s/%s%s",
                dsp_files_dir,
                savediag_model->dsp_fname_pt1,
                savediag_model->dsp_fname_pt2);

            /* Open storage and allocate space for the file*/
            storage = furi_record_open(RECORD_STORAGE);
            file = storage_file_alloc(storage);

            /* Create the destination directory */
            if(storage_simply_mkdir(storage, dsp_files_dir)) {
                /* Attempt to open the DSP file */
                if(storage_file_open(
                       file, savediag_model->dsp_fpath, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
                    /* Write the DSP file */
                    bytes_to_write = 0;
                    i = 0;
                    while(i < savediag_model->lrf_diag.nb_vals) {
                        /* If we're at the last value in the header, create a date / time
               marker */
                        if(i == savediag_model->lrf_diag.vals[0])
                            snprintf(
                                savediag_model->spstr + bytes_to_write,
                                sizeof(savediag_model->spstr) - bytes_to_write,
                                "%02d/%02d/%04d %02d:%02d:%02d\r\n",
                                datetime.day,
                                datetime.month,
                                datetime.year,
                                datetime.hour,
                                datetime.minute,
                                datetime.second);

                        /* Otherwise transform the value into a signed or unsigned zero-
               padded number string depending on the version of the firmware */
                        else {
                            val = savediag_model->ident.is_fw_newer_than_x4 ?
                                      (int16_t)savediag_model->lrf_diag.vals[i] :
                                      (uint16_t)savediag_model->lrf_diag.vals[i];

                            snprintf(
                                savediag_model->spstr + bytes_to_write,
                                sizeof(savediag_model->spstr) - bytes_to_write,
                                "%s%05d\r\n",
                                val < 0 ? "-" : "",
                                abs(val));
                        }

                        bytes_to_write += strlen(savediag_model->spstr + bytes_to_write);

                        i++;

                        /* Write into the file in batches of 30 strings */
                        if(i % 30 == 0 || i == savediag_model->lrf_diag.nb_vals) {
                            bytes_written =
                                storage_file_write(file, &savediag_model->spstr, bytes_to_write);
                            total_bytes_written += bytes_written;

                            /* If all the bytes couldn't be written, stop and report an
                 error */
                            if(bytes_written != bytes_to_write) {
                                FURI_LOG_I(
                                    TAG,
                                    "Wrote %ld bytes to DSP file %s but %ld "
                                    "expected",
                                    bytes_written,
                                    savediag_model->dsp_fpath,
                                    bytes_to_write);

                                snprintf(
                                    savediag_model->status_msg1,
                                    sizeof(savediag_model->status_msg1),
                                    "Error!");

                                snprintf(
                                    savediag_model->status_msg2,
                                    sizeof(savediag_model->status_msg2),
                                    "Error writing %s",
                                    savediag_model->dsp_fname_pt1);

                                snprintf(
                                    savediag_model->status_msg3,
                                    sizeof(savediag_model->status_msg3),
                                    savediag_model->dsp_fname_pt2);

                                break;
                            }

                            bytes_to_write = 0;
                        }

                        /* Get the current timestamp */
                        now_ms = furi_get_tick();

                        /* Calculate the second half of the progress: saving */
                        savediag_model->progress =
                            ((float)i / (float)savediag_model->lrf_diag.total_vals) / 2 + 0.5;

                        /* Should we update the display? */
                        if(ms_tick_time_diff_ms(now_ms, last_update_display) >
                           DIAG_PROGRESS_UPDATE_EVERY) {
                            /* Trigger a save diagnostic view redraw to update the progress
                 bar */
                            with_view_model(
                                app->savediag_view,
                                SaveDiagModel * _model,
                                { UNUSED(_model); },
                                true);
                            last_update_display = now_ms;
                        }
                    }

                    /* Close the DSP file */
                    storage_file_close(file);
                }

                /* Error opening the DSP file: report an error */
                else {
                    FURI_LOG_I(
                        TAG, "Could not open DSP file %s for writing", savediag_model->dsp_fpath);

                    snprintf(
                        savediag_model->status_msg1,
                        sizeof(savediag_model->status_msg1),
                        "Error!");

                    snprintf(
                        savediag_model->status_msg2,
                        sizeof(savediag_model->status_msg2),
                        "Could not open %s",
                        savediag_model->dsp_fname_pt1);

                    snprintf(
                        savediag_model->status_msg3,
                        sizeof(savediag_model->status_msg3),
                        savediag_model->dsp_fname_pt2);
                }
            }

            /* Error creating the destination directory */
            else {
                FURI_LOG_I(TAG, "Could not create destination directory %s", dsp_files_dir);

                snprintf(
                    savediag_model->status_msg1, sizeof(savediag_model->status_msg1), "Error!");

                snprintf(
                    savediag_model->status_msg2,
                    sizeof(savediag_model->status_msg2),
                    "Could not create directory");

                snprintf(
                    savediag_model->status_msg3,
                    sizeof(savediag_model->status_msg3),
                    dsp_files_dir);
            }

            /* Free the file and close storage */
            storage_file_free(file);
            furi_record_close(RECORD_STORAGE);

            /* If the file was written without error, report success */
            if(!savediag_model->status_msg1[0]) {
                FURI_LOG_I(
                    TAG,
                    "%ld bytes saved in file %s",
                    total_bytes_written,
                    savediag_model->dsp_fpath);

                snprintf(savediag_model->status_msg1, sizeof(savediag_model->status_msg1), "OK");

                snprintf(
                    savediag_model->status_msg2,
                    sizeof(savediag_model->status_msg2),
                    "Data saved in %s",
                    savediag_model->dsp_fname_pt1);

                snprintf(
                    savediag_model->status_msg3,
                    sizeof(savediag_model->status_msg3),
                    savediag_model->dsp_fname_pt2);
            }
        }

        /* If the LRF identification data is missing, report an error */
        else {
            FURI_LOG_I(TAG, "LRF identification not received");

            snprintf(savediag_model->status_msg1, sizeof(savediag_model->status_msg1), "Error!");

            snprintf(
                savediag_model->status_msg2,
                sizeof(savediag_model->status_msg2),
                "Missing LRF identification");
        }

        savediag_model->save_in_progress = false;
    }

    /* Trigger a save diagnostic view redraw */
    with_view_model(
        app->savediag_view, SaveDiagModel * _model, { UNUSED(_model); }, true);
}

/** Save diagnostic view enter callback **/
void savediag_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;

    with_view_model(
        app->savediag_view,
        SaveDiagModel * savediag_model,
        {
            /* Start the UART at the correct baudrate */
            start_uart(app->lrf_serial_comm_app, app->config.baudrate);

            /* Setup the callback to receive decoded LRF identification frames */
            set_lrf_ident_handler(app->lrf_serial_comm_app, lrf_ident_handler, app);

            /* Setup the callback to receive diagnostic data */
            set_diag_data_handler(app->lrf_serial_comm_app, diag_data_handler, app);

            /* Let the LRF serial communication thread use the larger shared
	     storage space so it can receive a complete diagnostic frame */
            enable_shared_storage_dec_buf(app->lrf_serial_comm_app, true);

            /* Invalidate the current identification - if any */
            savediag_model->has_ident = false;

            /* Clear the progress */
            savediag_model->progress = -1;
            savediag_model->download_in_progress = false;
            savediag_model->save_in_progress = false;

            /* Clear the status message */
            savediag_model->status_msg1[0] = 0;
            savediag_model->status_msg2[0] = 0;
            savediag_model->status_msg3[0] = 0;

            /* Send a send-identification-frame command */
            send_lrf_command(app->lrf_serial_comm_app, send_ident);

            /* Send a read-diagnostic-data command */
            send_lrf_command(app->lrf_serial_comm_app, read_diag);
        },
        false);
}

/** Save diagnostic view exit callback **/
void savediag_view_exit_callback(void* ctx) {
    App* app = (App*)ctx;

    /* Don't let the LRF serial communication thread use the larger shared storage
     space anymore */
    enable_shared_storage_dec_buf(app->lrf_serial_comm_app, false);

    /* Unset the callback to receive diagnostic data */
    set_diag_data_handler(app->lrf_serial_comm_app, NULL, app);

    /* Unset the callback to receive decoded LRF identification frames */
    set_lrf_ident_handler(app->lrf_serial_comm_app, NULL, app);

    /* Stop the UART */
    stop_uart(app->lrf_serial_comm_app);
}

/** Draw callback for the save diagnostic view **/
void savediag_view_draw_callback(Canvas* canvas, void* model) {
    SaveDiagModel* savediag_model = (SaveDiagModel*)model;
    uint8_t x;

    /* Do we have a progress bar to display? */
    if(savediag_model->progress >= 0) {
        /* Draw a frame for the download progress bar */
        canvas_draw_frame(canvas, 0, 0, 128, 7);

        /* Draw the progress bar */
        x = 2 + 123.0 * savediag_model->progress;
        canvas_draw_line(canvas, 2, 2, x, 2);
        canvas_draw_line(canvas, 2, 3, x, 3);
        canvas_draw_line(canvas, 2, 4, x, 4);
    }

    /* Do we have a status message to display? */
    if(savediag_model->status_msg1[0]) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 0, 21, savediag_model->status_msg1);
    }

    if(savediag_model->status_msg2[0]) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 34, savediag_model->status_msg2);
    }

    if(savediag_model->status_msg3[0]) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 0, 43, savediag_model->status_msg3);
    }

    /* If no operation  is in progress, print the OK button symbol followed
     by "Read" in a frame at the right-hand side */
    if(!savediag_model->download_in_progress && !savediag_model->save_in_progress) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_frame(canvas, 77, 52, 51, 12);
        canvas_draw_icon(canvas, 79, 54, &I_ok_button);
        canvas_draw_str(canvas, 102, 62, "Save");
    }

    /* Draw a dividing line between the LRF information and the bottom line */
    canvas_draw_line(canvas, 0, 48, 128, 48);
}

/** Input callback for the save diagnostic view **/
bool savediag_view_input_callback(InputEvent* evt, void* ctx) {
    App* app = (App*)ctx;
    SaveDiagModel* savediag_model = view_get_model(app->savediag_view);

    /* If diagnostic data is being saved, disable all user input altogether */
    if(savediag_model->save_in_progress) {
        FURI_LOG_D(TAG, "User input disabled while diagnostic data is being saved");
        return true;
    }

    /* Did the user press the OK button? */
    if(evt->type == InputTypePress && evt->key == InputKeyOk) {
        /* If no diagnostic data is being downloaded, trigger a new save sequence */
        if(!savediag_model->download_in_progress) {
            FURI_LOG_D(TAG, "OK button pressed");

            /* Invalidate the current identification - if any */
            savediag_model->has_ident = false;

            /* Clear the progress */
            savediag_model->progress = -1;
            savediag_model->download_in_progress = false;
            savediag_model->save_in_progress = false;

            /* Clear the status message */
            savediag_model->status_msg1[0] = 0;
            savediag_model->status_msg2[0] = 0;
            savediag_model->status_msg3[0] = 0;

            /* Trigger a save diagnostic view redraw to clear the information
         currently displayed - if any */
            with_view_model(
                app->savediag_view, SaveDiagModel * _model, { UNUSED(_model); }, true);

            /* Send a send-identification-frame command */
            send_lrf_command(app->lrf_serial_comm_app, send_ident);

            /* Send a read-diagnostic-data command */
            send_lrf_command(app->lrf_serial_comm_app, read_diag);
        }

        else
            FURI_LOG_D(
                TAG,
                "OK button pressed, but diagnostic data is being "
                "downloaded, so ignored");

        return true;
    }

    /* We haven't handled this event */
    return false;
}
