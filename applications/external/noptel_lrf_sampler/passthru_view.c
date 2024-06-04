/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * USB serial passthrough view
***/

/*** Includes ***/
#include <cli/cli.h>
#include <cli/cli_vcp.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>

#include "common.h"
#include "noptel_lrf_sampler_icons.h" /* Generated from images in assets */

/*** Defines ***/
#define VCP_RX_STREAM_BUF_SIZE 1024

/*** Forward declarations ***/
static void vcp_on_cdc_tx_complete(void*);
static void vcp_on_cdc_rx(void*);
static void vcp_on_state_change(void*, uint8_t);
static void vcp_on_line_config(void*, struct usb_cdc_line_coding*);

/*** Virtual COM port callbacks ***/
static CdcCallbacks cdc_callbacks =
    {vcp_on_cdc_tx_complete, vcp_on_cdc_rx, vcp_on_state_change, NULL, vcp_on_line_config};

/*** Virtual COM port RX/TX thread events ***/
typedef enum { stop = 1, data_avail = 2, data_to_send = 4 } vcp_rx_tx_thread_evts;

/*** Hexadecimal digit icons ***/
static const Icon* hex_icons[] = {
    &I_hex_0,
    &I_hex_1,
    &I_hex_2,
    &I_hex_3,
    &I_hex_4,
    &I_hex_5,
    &I_hex_6,
    &I_hex_7,
    &I_hex_8,
    &I_hex_9,
    &I_hex_A,
    &I_hex_B,
    &I_hex_C,
    &I_hex_D,
    &I_hex_E,
    &I_hex_F};

/*** Routines ***/

/** Time difference in milliseconds between system ticks in milliseconds,
    taking the timestamp overflow into account **/
static uint32_t ms_tick_time_diff_ms(uint32_t tstamp1, uint32_t tstamp2) {
    if(tstamp1 >= tstamp2)
        return tstamp1 - tstamp2;

    else
        return 0xffffffff - tstamp2 + 1 + tstamp1;
}

/** LRF raw data handler
    Called when raw data is read by the LRF serial communication app **/
static void lrf_raw_data_handler(uint8_t* data, uint16_t len, void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);

    /* Send the data to the UART receive stream buffer */
    furi_stream_buffer_send(passthru_model->uart_rx_stream, data, len, 0);

    /* Tell the virtual COM port RX/TX thread it has data to send */
    furi_thread_flags_set(passthru_model->vcp_rx_tx_thread_id, data_to_send);
}

/** Mirror the state of the virtual COM port on the UART: open the UART if the
    virtual COM port is connected and set the same baudrate **/
static void mirror_vcp_on_uart(App* app, PassthruModel* passthru_model) {
    uint8_t i;

    /* Is the virtual COM port connected */
    if(passthru_model->enabled && passthru_model->vcp_connected)

        /* Check that the virtual COM port has a baudrate the LRF supports */
        for(i = 0; i < nb_config_baudrate_values &&
                   passthru_model->vcp_config->dwDTERate != config_baudrate_values[i];
            i++)
            ;

    else
        i = nb_config_baudrate_values;

    /* Should we start or reconfigure the UART? */
    if(i < nb_config_baudrate_values) {
        /* Get the human-readable baudrate name */
        passthru_model->uart_baudrate_name = config_baudrate_names[i];

        /* Is the UART stopped? */
        if(!passthru_model->uart_baudrate) {
            /* Start the UART at the correct baudrate */
            start_uart(app->lrf_serial_comm_app, passthru_model->vcp_config->dwDTERate);

            /* Setup the callback to receive raw LRF data */
            set_lrf_raw_data_handler(app->lrf_serial_comm_app, lrf_raw_data_handler, app);

            /* Update the display */
            passthru_model->update_display = true;
        }

        /* The UART is already started */
        else {
            /* Change the UART's baudrate if needed */
            if(passthru_model->uart_baudrate != passthru_model->vcp_config->dwDTERate) {
                set_uart_baudrate(app->lrf_serial_comm_app, passthru_model->vcp_config->dwDTERate);

                /* Update the display */
                passthru_model->update_display = true;
            }
        }

        passthru_model->uart_baudrate = passthru_model->vcp_config->dwDTERate;
    }

    /* We should stop the UART */
    else {
        /* No baudrate name */
        passthru_model->uart_baudrate_name = NULL;

        /* Is the UART started? */
        if(passthru_model->uart_baudrate) {
            /* Unset the callback to receive raw LRF data */
            set_lrf_raw_data_handler(app->lrf_serial_comm_app, NULL, app);

            /* Stop the UART */
            stop_uart(app->lrf_serial_comm_app);
            passthru_model->uart_baudrate = 0;

            /* Update the display */
            passthru_model->update_display = true;
        }
    }
}

/** Virtual COM port callback for when the transmission is complete */
static void vcp_on_cdc_tx_complete(void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);

    /* Release the virtual COM port TX semaphore */
    furi_semaphore_release(passthru_model->vcp_tx_sem);
}

/** Virtual COM port callback for when data is received */
static void vcp_on_cdc_rx(void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);

    /* Get the data from the virtual COM port */
    passthru_model->vcp_rx_buf_len = furi_hal_cdc_receive(
        app->config.passthru_chan, passthru_model->vcp_rx_buf, sizeof(passthru_model->vcp_rx_buf));

    /* Send the data to the virtual COM port receive stream buffer */
    furi_stream_buffer_send(
        passthru_model->vcp_rx_stream,
        passthru_model->vcp_rx_buf,
        passthru_model->vcp_rx_buf_len,
        0);

    /* Tell the virtual COM port RX/TX thread that data is available */
    furi_thread_flags_set(passthru_model->vcp_rx_tx_thread_id, data_avail);
}

/** Virtual COM port callback for when the port's connected state changes */
static void vcp_on_state_change(void* ctx, uint8_t state) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);

    passthru_model->vcp_connected = state ? true : false;

    /* Mirror the virtual COM port on the UART */
    mirror_vcp_on_uart(app, passthru_model);
}

/** Virtual COM port callback for when the port's configuration changes */
static void vcp_on_line_config(void* ctx, struct usb_cdc_line_coding* vcp_cfg) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);

    /* Get the new virtual COM port configuration */
    passthru_model->vcp_config = vcp_cfg;

    /* Mirror the virtual COM port on the UART */
    mirror_vcp_on_uart(app, passthru_model);
}

/** Serial traffic logger */
static void log_serial_bytes(
    PassthruModel* passthru_model,
    bool to_lrf,
    uint8_t* bytes,
    uint16_t nb_bytes) {
    uint16_t i, j;

    /* Acquire the mutex to get exclusive access to the traffic log */
    furi_check(
        furi_mutex_acquire(passthru_model->traffic_log_mutex, FuriWaitForever) == FuriStatusOk);

    /* Add the bytes to the traffic log ring buffer to display in the view's
     traffic display screen */
    for(i = 0; i < nb_bytes; i++) {
        j = bytes[i] | (to_lrf ? 0x100 : 0);

        if(passthru_model->traffic_log_len < COUNT_OF(passthru_model->traffic_log))
            passthru_model->traffic_log[passthru_model->traffic_log_len++] = j;
        else {
            passthru_model->traffic_log[passthru_model->traffic_log_start++] = j;
            if(passthru_model->traffic_log_start == COUNT_OF(passthru_model->traffic_log))
                passthru_model->traffic_log_start = 0;
        }
    }

    /* Release access to the traffic log */
    furi_check(furi_mutex_release(passthru_model->traffic_log_mutex) == FuriStatusOk);

    /* Should we log the relayed bytes in the CLI? */
    if(passthru_model->traffic_logging_prefix[0]) {
        /* Initialize the log string */
        passthru_model->traffic_logging_prefix[0] = to_lrf ? '>' : '<';
        snprintf(
            passthru_model->spstr2,
            sizeof(passthru_model->spstr2),
            "%s:",
            passthru_model->traffic_logging_prefix);
        j = strlen(passthru_model->spstr2);

        /* Add the bytes as hex values */
        for(i = 0; i < nb_bytes; i++) {
            snprintf(
                passthru_model->spstr2 + j, sizeof(passthru_model->spstr2) - j, " %02x", bytes[i]);
            j += 3;
        }

        /* Log the line */
        FURI_LOG_T(TAG, passthru_model->spstr2);
    }

    /* Update the display */
    passthru_model->update_display = true;
}

/** Virtual COM port RX/TX thread **/
static int32_t vcp_rx_tx_thread(void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);
    uint32_t evts;
    uint32_t now_ms;

    /* Trigger the first passthrough view redraw */
    with_view_model(
        app->passthru_view, PassthruModel * _model, { UNUSED(_model); }, true);
    passthru_model->last_display_update_tstamp = furi_get_tick();
    passthru_model->update_display = false;

    while(1) {
        /* Get events */
        evts = furi_thread_flags_wait(
            stop | data_avail | data_to_send, FuriFlagWaitAny, passthru_view_update_every);

        /* Check for errors */
        furi_check(((evts & FuriFlagError) == 0) || (evts == FuriFlagErrorTimeout));

        /* Get the current timestamp */
        now_ms = furi_get_tick();

        /* Should we update the display? */
        if(passthru_model->update_display &&
           ms_tick_time_diff_ms(now_ms, passthru_model->last_display_update_tstamp) >=
               passthru_view_update_every) {
            /* Trigger a passthrough view redraw */
            with_view_model(
                app->passthru_view, PassthruModel * _model, { UNUSED(_model); }, true);
            passthru_model->last_display_update_tstamp = now_ms;
            passthru_model->update_display = false;
        }

        /* If we timed out, we have no events to process */
        if(evts == FuriFlagErrorTimeout) continue;

        /* Should we stop the thread? */
        if(evts & stop) {
            /* Clear the information about the serial traffic on the display before
         stopping, so it doesn't show up briefly before being reset when the
         user reenters the view */
            passthru_model->show_serial_traffic = false;
            with_view_model(
                app->passthru_view, PassthruModel * _model, { UNUSED(_model); }, true);

            break;
        }

        /* Should we relay data from the virtual COM port to the UART? */
        if(evts & data_avail) {
            /* Get the data from the virtual COM port receive stream buffer */
            passthru_model->uart_tx_buf_len = furi_stream_buffer_receive(
                passthru_model->vcp_rx_stream,
                passthru_model->uart_tx_buf,
                sizeof(passthru_model->uart_tx_buf),
                0);

            /* Did we actually get something? */
            if(passthru_model->uart_tx_buf_len) {
                /* Is the UART started, is the passthrough enabled and is the virtual
           COM port connected? */
                if(passthru_model->uart_baudrate && passthru_model->enabled &&
                   passthru_model->vcp_connected) {
                    /* Relay the data to the UART */
                    uart_tx(
                        app->lrf_serial_comm_app,
                        passthru_model->uart_tx_buf,
                        passthru_model->uart_tx_buf_len);

                    /* Log the relayed bytes */
                    log_serial_bytes(
                        passthru_model,
                        true,
                        passthru_model->uart_tx_buf,
                        passthru_model->uart_tx_buf_len);

                    /* Update the counter of bytes sent to the LRF */
                    passthru_model->total_bytes_sent += passthru_model->uart_tx_buf_len;
                }

                /* Tell ourselves that more data is available */
                furi_thread_flags_set(passthru_model->vcp_rx_tx_thread_id, data_avail);
            }
        }

        /* Should we relay data from UART to the virtual COM port? */
        if(evts & data_to_send) {
            /* Get the data from the UART receive stream buffer */
            passthru_model->vcp_tx_buf_len = furi_stream_buffer_receive(
                passthru_model->uart_rx_stream,
                passthru_model->vcp_tx_buf,
                sizeof(passthru_model->vcp_tx_buf),
                0);

            /* Do we have something to send? */
            if(passthru_model->vcp_tx_buf_len) {
                /* If the UART is started, the passthrough is enabled and the virtual
	   COM port is connected, try to acquire the semaphore so we block at
	   the next round until the transmission is complete. Only try
           for a while so we don't get hung up */
                if(passthru_model->uart_baudrate && passthru_model->enabled &&
                   passthru_model->vcp_connected &&
                   furi_semaphore_acquire(passthru_model->vcp_tx_sem, 500) == FuriStatusOk) {
                    /* Send the UART data */
                    furi_hal_cdc_send(
                        app->config.passthru_chan,
                        passthru_model->vcp_tx_buf,
                        passthru_model->vcp_tx_buf_len);
                    passthru_model->vcp_last_sent = passthru_model->vcp_tx_buf_len;

                    /* Update the counter of bytes received from the LRF */
                    passthru_model->total_bytes_recv += passthru_model->vcp_last_sent;

                    /* Log the relayed bytes */
                    log_serial_bytes(
                        passthru_model,
                        false,
                        passthru_model->vcp_tx_buf,
                        passthru_model->vcp_last_sent);
                }

                /* The passthrough is disabled or we failed to acquire the semaphore,
           so we didn't send anything */
                else
                    passthru_model->vcp_last_sent = 0;

                /* Tell ourselves that more data is available */
                furi_thread_flags_set(passthru_model->vcp_rx_tx_thread_id, data_to_send);
            }

            /* We have nothing to send */
            else {
                /* Special usbd_ep_write oddity: if the last block of bytes sent was
           the maximum size allowed (64 bytes), the actual transfer is held up
           and we need to send 0 bytes to trigger the actual data transfer */
                if(passthru_model->vcp_last_sent > sizeof(passthru_model->vcp_tx_buf_len)) {
                    /* If the UART is started, the passthrough is enabled and the virtual
	     COM port is connected, try to acquire the semaphore so we block at
	     the next round until the transmission is complete. Only try
             for a while so we don't get hung up */
                    if(passthru_model->uart_baudrate && passthru_model->enabled &&
                       passthru_model->vcp_connected &&
                       furi_semaphore_acquire(passthru_model->vcp_tx_sem, 500) == FuriStatusOk) {
                        /* Send 0 bytes */
                        furi_hal_cdc_send(app->config.passthru_chan, NULL, 0);
                        passthru_model->vcp_last_sent = 0;
                    }
                }
            }
        }
    }

    return 0;
}

/** USB serial passthrough view enter callback
    Configure the virtual COM port and start the RX/TX thread */
void passthru_view_enter_callback(void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);
    Cli* cli;

    furi_hal_usb_unlock();

    /* Get the current state of the USB interface */
    passthru_model->usb_interface_state_save = furi_hal_usb_get_config();

    /* Close the CLI */
    cli = furi_record_open(RECORD_CLI);
    cli_session_close(cli);
    furi_record_close(RECORD_CLI);

    /* Wait a bit before fiddling with the USB interface, otherwise we might crash
     for some reason */
    furi_delay_ms(100);

    /* Use CDC channel 0? */
    if(app->config.passthru_chan == 0) {
        /* If the USB interface is not configured as CDC single channel,
       reconfigure it */
        if(passthru_model->usb_interface_state_save != &usb_cdc_single)
            furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);

        /* The USB interface is already configured as CDC single channel:
       reinitialize if to knock out any existing serial connection -
       particularly qFlipper, so if doesn't sit there idling on a dead
       connection but actively starts trying to reconnect instead */
        else
            furi_hal_usb_reinit();
    }

    /* Use CDC channel 1 */
    else {
        /* If the USB interface is not configured as CDC dual channel,
       reconfigure it */
        if(passthru_model->usb_interface_state_save != &usb_cdc_dual)
            furi_check(furi_hal_usb_set_config(&usb_cdc_dual, NULL) == true);

        /* The USB interface is already configured as CDC dual channel:
       reinitialize if to knock out any existing serial connection -
       particularly qFlipper, so if doesn't sit there idling on a dead
       connection but actively starts trying to reconnect instead */
        else
            furi_hal_usb_reinit();

        /* Reopen the CLI */
        cli = furi_record_open(RECORD_CLI);
        cli_session_open(cli, &cli_vcp);
        furi_record_close(RECORD_CLI);
    }

    /* Get the current virtual COM port configuration */
    passthru_model->vcp_config = furi_hal_cdc_get_port_settings(app->config.passthru_chan);

    /* Reset the serial traffic counters and enable showing them in the view */
    passthru_model->total_bytes_sent = 0;
    passthru_model->total_bytes_recv = 0;

    /* Create the mutex to access the traffic log */
    passthru_model->traffic_log_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    /* Empty the traffic log */
    passthru_model->traffic_log_start = 0;
    passthru_model->traffic_log_len = 0;

    /* Show the serial traffic information and start at the first screen */
    passthru_model->show_serial_traffic = true;
    passthru_model->screen = 0;

    /* Start out with the passthrough enabled, and assume the virtual
             COM port isn't connected */
    passthru_model->enabled = true;
    passthru_model->vcp_connected = false;

    /* Mirror the virtual COM port on the UART */
    mirror_vcp_on_uart(app, passthru_model);

    /* Nothing sent to the virtual COM port yet */
    passthru_model->vcp_last_sent = 0;

    /* Initialise the serial traffic logging prefix to an empty string if
     we don't have a console to log to, or a prefix with no direction */
    if(app->config.passthru_chan == 0)
        passthru_model->traffic_logging_prefix[0] = 0;
    else
        snprintf(
            passthru_model->traffic_logging_prefix,
            sizeof(passthru_model->traffic_logging_prefix),
            " LRF");

    /* Create the virtual COM port TX semaphore, to avoid sending data
     before the previous transmission is finished */
    passthru_model->vcp_tx_sem = furi_semaphore_alloc(1, 1);

    /* Allocate space for the UART receive stream buffer */
    passthru_model->uart_rx_stream = furi_stream_buffer_alloc(UART_RX_BUF_SIZE, 1);

    /* Allocate space for the virtual COM port receive stream buffer */
    passthru_model->vcp_rx_stream = furi_stream_buffer_alloc(VCP_RX_STREAM_BUF_SIZE, 1);

    /* Allocate space for the virtual COM port RX/TX thread */
    passthru_model->vcp_rx_tx_thread = furi_thread_alloc();

    /* Initialize the virtual COM port RX/TX thread */
    furi_thread_set_name(passthru_model->vcp_rx_tx_thread, "vcp_rx_tx");
    furi_thread_set_stack_size(passthru_model->vcp_rx_tx_thread, 1024);
    furi_thread_set_context(passthru_model->vcp_rx_tx_thread, app);
    furi_thread_set_callback(passthru_model->vcp_rx_tx_thread, vcp_rx_tx_thread);

    /* Start the virtual COM port RX/TX thread */
    furi_thread_start(passthru_model->vcp_rx_tx_thread);

    /* Get the virtual COM port RX/TX thread ID */
    passthru_model->vcp_rx_tx_thread_id = furi_thread_get_id(passthru_model->vcp_rx_tx_thread);

    /* Set the virtual COM port callbacks */
    furi_hal_cdc_set_callbacks(app->config.passthru_chan, &cdc_callbacks, app);
}

/** USB serial passthrough view exit callback
    Stop the virtual COM port RX/TX thread and release the virtual COM port **/
void passthru_view_exit_callback(void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);
    Cli* cli;

    /* If the second screen is displayed, set the backlight back to automatic */
    if(passthru_model->screen == 1) set_backlight(&app->backlight_control, BL_AUTO);

    /* If the UART is started, unset the callback to receive raw LRF data and
     stop the UART */
    if(passthru_model->uart_baudrate) {
        set_lrf_raw_data_handler(app->lrf_serial_comm_app, NULL, app);
        stop_uart(app->lrf_serial_comm_app);
    }

    /* Unset the virtual COM port callbacks */
    furi_hal_cdc_set_callbacks(app->config.passthru_chan, NULL, NULL);

    /* Stop and free the virtual COM port RX/TX thread */
    furi_thread_flags_set(passthru_model->vcp_rx_tx_thread_id, stop);
    furi_thread_join(passthru_model->vcp_rx_tx_thread);
    furi_thread_free(passthru_model->vcp_rx_tx_thread);

    /* Free the virtual COM port receive stream buffer */
    furi_stream_buffer_free(passthru_model->vcp_rx_stream);

    /* Free the UART receive stream buffer */
    furi_stream_buffer_free(passthru_model->uart_rx_stream);

    /* Free the virtual COM port TX semaphore */
    furi_semaphore_free(passthru_model->vcp_tx_sem);

    /* Free the mutex to access the traffic log */
    furi_mutex_free(passthru_model->traffic_log_mutex);

    /* Close the CLI */
    cli = furi_record_open(RECORD_CLI);
    cli_session_close(cli);
    furi_record_close(RECORD_CLI);

    /* Wait a bit before fiddling with the USB interface, otherwise we might crash
     for some reason */
    furi_delay_ms(100);

    /* Restore the USB interface as we found it */
    furi_check(furi_hal_usb_set_config(passthru_model->usb_interface_state_save, NULL) == true);

    /* Restart the CLI */
    cli = furi_record_open(RECORD_CLI);
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);
}

/** Draw callback for the USB serial passthrough view **/
void passthru_view_draw_callback(Canvas* canvas, void* model) {
    PassthruModel* passthru_model = (PassthruModel*)model;
    int16_t x, y;
    uint8_t i, j;
    bool is_byte_sent;
    bool was_byte_sent;
    bool video_reversed;

    /* Should we draw any information about the serial traffic at all? */
    if(passthru_model->show_serial_traffic) {
        /* Which screen should we draw? */
        switch(passthru_model->screen) {
        /* Draw the screen showing the traffic counters */
        case 0:

            /* Print the traffic counters in the FontBigNumber font */
            canvas_set_font(canvas, FontBigNumbers);

            /* Print the number of bytes sent to the LRF */
            snprintf(
                passthru_model->spstr1,
                sizeof(passthru_model->spstr1),
                "%ld",
                passthru_model->total_bytes_sent);
            canvas_draw_str_aligned(
                canvas, 64, 14, AlignCenter, AlignBottom, passthru_model->spstr1);

            /* Print the number of bytes received from the LRF */
            snprintf(
                passthru_model->spstr1,
                sizeof(passthru_model->spstr1),
                "%ld",
                passthru_model->total_bytes_recv);
            canvas_draw_str_aligned(
                canvas, 64, 46, AlignCenter, AlignBottom, passthru_model->spstr1);

            /* Draw the icon to show the directions of serial traffic in-between */
            canvas_draw_icon(canvas, 0, 16, &I_flipper_lrf_serial_traffic);

            /* Draw a dividing line between the serial traffic stats  and the bottom
           line */
            canvas_draw_line(canvas, 0, 48, 128, 48);

            /* Draw a right arrow at the top right */
            canvas_draw_icon(canvas, 124, 0, &I_arrow_right);

            break;

        /* Draw the screen showing the last bytes sent and received */
        case 1:

            /* Acquire the mutex to get exclusive access to the traffic log */
            furi_check(
                furi_mutex_acquire(passthru_model->traffic_log_mutex, FuriWaitForever) ==
                FuriStatusOk);

            /* Make a copy of the traffic log that we'll work on later so we can
           release the mutex asap and avoid holding up the virtual COM port
           RX/TX thread */
            memcpy(
                passthru_model->traffic_log_copy,
                passthru_model->traffic_log,
                sizeof(passthru_model->traffic_log));
            passthru_model->traffic_log_start_copy = passthru_model->traffic_log_start;
            passthru_model->traffic_log_len_copy = passthru_model->traffic_log_len;

            /* Release access to the traffic log */
            furi_check(furi_mutex_release(passthru_model->traffic_log_mutex) == FuriStatusOk);

            /* Start displaying hex values after a space reserved for the left
           arrow at the top-left corner */
            is_byte_sent = false;
            video_reversed = false;
            x = 9;
            y = 0;

            /* Display the traffig log bytes as hex values */
            for(j = 0, i = passthru_model->traffic_log_start_copy;
                j < passthru_model->traffic_log_len_copy;
                j++) {
                was_byte_sent = is_byte_sent;
                is_byte_sent = passthru_model->traffic_log_copy[i] & 0x100 ? true : false;

                /* Display the bytes sent to the LRF in reverse video, and the bytes
             received from the LRF normally */
                if(is_byte_sent != video_reversed) {
                    canvas_invert_color(canvas);
                    video_reversed = !video_reversed;
                }

                /* If both the previous byte and this byte are in reverse video and on
             the same line, join them */
                if(x >= 9 && was_byte_sent && is_byte_sent)
                    canvas_draw_icon(canvas, x - 1, y, &I_inter_bytes_space);

                /* Print the byte's most significant nibble */
                canvas_draw_icon(
                    canvas, x, y, hex_icons[(passthru_model->traffic_log_copy[i] & 0xf0) >> 4]);

                /* Print the byte's least significant nibble */
                canvas_draw_icon(
                    canvas, x + 4, y, hex_icons[passthru_model->traffic_log_copy[i] & 0x0f]);

                /* Move to the next spot right of this displayed byte, or at the
             beginning of the next line */
                x += 10;
                if(x > 127) {
                    x = -1;
                    y += 7;
                }

                /* Next byte to display - go around the ring buffer if needed */
                if(++i == COUNT_OF(passthru_model->traffic_log_copy)) i = 0;
            }

            /* Restore normal video */
            if(video_reversed) canvas_invert_color(canvas);

            /* Draw a dividing line between the serial traffic stats and the
           bottom line that's slightly lower than normal to accommodate as many
           hex values as possible */
            canvas_draw_line(canvas, 0, 50, 128, 50);

            /* Draw a left arrow at the top left */
            canvas_draw_icon(canvas, 0, 0, &I_arrow_left);

            break;
        }
    }

    /* Don't draw any information about the serial traffic at all */
    else {
        /* Just draw a dividing line between the serial traffic stats and the
       bottom line */
        canvas_draw_line(canvas, 0, 48, 128, 48);
    }

    /* Show the connected state of the virtual COM port using an icon at the
     bottom left */
    canvas_draw_icon(
        canvas, 0, 54, passthru_model->vcp_connected ? &I_connected : &I_disconnected);

    /* Finally print the bottom line information in the FontPrimary font
     (bold, proportional) */
    canvas_set_font(canvas, FontPrimary);

    /* Print the UART baudrate next to the connected icon if the virtual COM port
     if connected and we have a baudrate to display */
    if(passthru_model->vcp_connected && passthru_model->uart_baudrate_name) {
        canvas_draw_str(canvas, 20, 62, passthru_model->uart_baudrate_name);
    }

    /* Print the OK button symbol followed by "Start" or "Stop" in a frame at the
     right-hand side depending on whether the passthrough is enabled or not */
    canvas_draw_frame(canvas, 77, 52, 51, 12);
    canvas_draw_icon(canvas, 79, 54, &I_ok_button);
    if(passthru_model->enabled)
        canvas_draw_str(canvas, 102, 62, "Stop");
    else
        canvas_draw_str(canvas, 102, 62, "Start");
}

/** Input callback for the USB serial passthrough view **/
bool passthru_view_input_callback(InputEvent* evt, void* ctx) {
    App* app = (App*)ctx;
    PassthruModel* passthru_model = view_get_model(app->passthru_view);
    bool evt_handled = false;

    /* Was the event a button press? */
    if(evt->type == InputTypePress) /* Which button was pressed? */
        switch(evt->key) {
        /* OK button: start or stop the passthrough */
        case InputKeyOk:
            FURI_LOG_D(TAG, "OK button pressed");

            passthru_model->enabled = !passthru_model->enabled;

            /* Mirror the virtual COM port on the UART */
            mirror_vcp_on_uart(app, passthru_model);

            break;

        /* Right button: go to the next screen */
        case InputKeyRight:
            FURI_LOG_D(TAG, "Right button pressed");

            /* If the second screen isn't already displayed, set the backlight on
           all the time */
            if(passthru_model->screen != 1) set_backlight(&app->backlight_control, BL_ON);

            passthru_model->screen = 1;
            evt_handled = true;
            break;

        /* Left button: go to the previous screen */
        case InputKeyLeft:
            FURI_LOG_D(TAG, "Left button pressed");

            /* If the first screen isn't already displayed, set the backlight back
           to automatic */
            if(passthru_model->screen != 0) set_backlight(&app->backlight_control, BL_AUTO);

            passthru_model->screen = 0;
            evt_handled = true;
            break;

        default:
            evt_handled = false;
        }

    /* If we haven't handled this event, return now */
    if(!evt_handled) return false;

    /* Update the display */
    passthru_model->update_display = true;

    /* We handled the event */
    return true;
}
