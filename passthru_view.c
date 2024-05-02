/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.6
 *
 * USB serial passthrough view
***/

/*** Includes ***/
#include <cli/cli.h>
#include <cli/cli_vcp.h>
#include <furi_hal.h>
#include <furi_hal_usb_cdc.h>

#include "common.h"
#include "noptel_lrf_sampler_icons.h"	/* Generated from images in assets */



/*** Defines ***/
#define VCP_DTR_BIT 0
#define VCP_RTS_BIT 1



/*** Forward declarations ***/
static void vcp_on_cdc_tx_complete(void *);
static void vcp_on_cdc_rx(void *);
static void vcp_state_callback(void *, uint8_t);
static void vcp_on_cdc_control_line(void *, uint8_t);
static void vcp_on_line_config(void *, struct usb_cdc_line_coding *);



/*** Virtual COM port callbacks ***/
static CdcCallbacks cdc_callbacks = {
  vcp_on_cdc_tx_complete,
  vcp_on_cdc_rx,
  vcp_state_callback,
  vcp_on_cdc_control_line,
  vcp_on_line_config
};



/** Virtual COM port RX/TX thread events **/
typedef enum {
  stop = 1,
  data_avail = 2,
  data_to_send = 4,
} vcp_rx_tx_thread_evts;



/*** Routines ***/

/** LRF raw data handler
    Called when raw data is read by the LRF serial communication app **/
static void lrf_raw_data_handler(uint8_t *data, uint16_t len, void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Wait until the previous transmission completes */
  furi_check(furi_semaphore_acquire(passthru_model->vcp_tx_sem, FuriWaitForever)
		== FuriStatusOk);

  /* Make a copy of the data to send */
  memcpy(passthru_model->uart_rx_buf, data, len);
  passthru_model->uart_rx_buf_len = len;

  /* Tell the virtual COM port RX/TX thread it has data to send */
  furi_thread_flags_set(furi_thread_get_id(passthru_model->vcp_rx_tx_thread),
						data_to_send);
}



/** Mirror the state of the virtual COM port on the UART: open the UART if the
    virtual COM port is open and set the same baudrate **/
static void mirror_vcp_on_uart(App *app, PassthruModel *passthru_model) {

  uint8_t i;

  /* Check that all the conditions are met to start or reconfigure the UART */
  if(passthru_model->enabled && passthru_model->vcp_connected &&
	passthru_model->vcp_open)

    /* Check that the virtual COM port has a baudrate the LRF supports */
    for(i=0; i < nb_config_baudrate_values &&
	passthru_model->vcp_config->dwDTERate != config_baudrate_values[i];
	i++);

  else
    i = nb_config_baudrate_values;

  /* Should we start or reconfigure the UART? */
  if(i < nb_config_baudrate_values) {

    /* Get the human-readable baudrate name */
    passthru_model->uart_baudrate_name = config_baudrate_names[i];

    /* Is the UART stopped? */
    if(!passthru_model->uart_baudrate) {

      /* Start the UART at the correct baudrate */
      start_uart(app->lrf_serial_comm_app,
			passthru_model->vcp_config->dwDTERate);

      /* Setup the callback to receive raw LRF data */
      set_lrf_raw_data_handler(app->lrf_serial_comm_app, lrf_raw_data_handler,
				app);
    }

    /* The UART is already started */
    else {

      /* Change the UART's baudrate if needed */
      if(passthru_model->uart_baudrate != passthru_model->vcp_config->dwDTERate)
        set_uart_baudrate(app->lrf_serial_comm_app,
				passthru_model->vcp_config->dwDTERate);
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
    }
  }
}



/** Virtual COM port callback for when transmission is complete */
static void vcp_on_cdc_tx_complete(void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Release the virtual COM port TX semaphore */
  furi_semaphore_release(passthru_model->vcp_tx_sem);
}



/** Virtual COM port callback for when data is received */
static void vcp_on_cdc_rx(void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Tell the virtual COM port RX/TX thread that data is available */
  furi_thread_flags_set(furi_thread_get_id(passthru_model->vcp_rx_tx_thread),
						data_avail);
}



/** Virtual COM port callback for when the port's connected state changes */
static void vcp_state_callback(void *ctx, uint8_t state) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  passthru_model->vcp_connected = state? true : false;

  /* Mirror the virtual COM port on the UART */
  mirror_vcp_on_uart(app, passthru_model);

  /* Update the display */
  passthru_model->update_display = true;
}



/** Virtual COM port callback for when RTS or DTR change state */
static void vcp_on_cdc_control_line(void *ctx, uint8_t state) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Something has opened the virtual COM port at the remote end if the virtual
     DTR line is asserted */
  passthru_model->vcp_open = state & (1 << VCP_DTR_BIT)? true : false;

  /* Mirror the virtual COM port on the UART */
  mirror_vcp_on_uart(app, passthru_model);

  /* Update the display */
  passthru_model->update_display = true;
}



/** Virtual COM port callback for when the port's configuration changes */
static void vcp_on_line_config(void *ctx, struct usb_cdc_line_coding *vpc_cfg) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Get the new virtual COM port configuration */
  passthru_model->vcp_config = vpc_cfg;

  /* Mirror the virtual COM port on the UART */
  mirror_vcp_on_uart(app, passthru_model);

  /* Update the display */
  passthru_model->update_display = true;
}



/** Virtual COM port RX/TX thread **/
static int32_t vcp_rx_tx_thread(void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);
  uint32_t evts;

  while(1) {

    /* Get until we get either a stop event of we received data */
    evts = furi_thread_flags_wait(stop | data_avail | data_to_send,
					FuriFlagWaitAny, FuriWaitForever);

    /* Check for errors */
    furi_check((evts & FuriFlagError) == 0);

    /* Should we stop the thread? */
    if(evts & stop)
      break;

    /* Should we relay data from the virtual COM port to the UART? */
    if(evts & data_avail) {

      /* Get the data from the virtual COM port */
      passthru_model->vcp_rx_buf_len =
		furi_hal_cdc_receive(passthru_vcp_channel,
					passthru_model->vcp_rx_buf,
					sizeof(passthru_model->vcp_rx_buf));

      /* Did we actually get something, and is the UART started? */
      if(passthru_model->vcp_rx_buf_len && passthru_model->uart_baudrate) {

        /* Relay the data to the UART */
        uart_tx(app->lrf_serial_comm_app,
		passthru_model->vcp_rx_buf, passthru_model->vcp_rx_buf_len);

        FURI_LOG_T(TAG, "UART < VDC: %d bytes", passthru_model->vcp_rx_buf_len);

        /* Update the counter of bytes sent to the LRF */
	passthru_model->total_bytes_sent += passthru_model->vcp_rx_buf_len;

        /* Update the display */
        passthru_model->update_display = true;
      }
    }

    /* Should we relay data from UART to the virtual COM port? */
    if(evts & data_to_send) {

      /* Relay the data to the virtual COM port */
      furi_hal_cdc_send(passthru_vcp_channel, passthru_model->uart_rx_buf,
			passthru_model->uart_rx_buf_len);

      /* Wait a bit to properly flush the TX buffer, otherwise we risk data
         corruption at high speed */
      furi_delay_ms(1);

      FURI_LOG_T(TAG, "UART > VDC: %d bytes", passthru_model->uart_rx_buf_len);

        /* Update the counter of bytes received from the LRF */
	passthru_model->total_bytes_recv += passthru_model->uart_rx_buf_len;

      /* Update the display */
      passthru_model->update_display = true;
    }
  }

  return 0;
}



/** USB serial passthrough view update timer callback **/
static void passthru_view_timer_callback(void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* Should the display be updated? */
  if(passthru_model->update_display) {

    FURI_LOG_I(TAG, "DISPLAY UPDATE");

    /* Trigger a passthrough view redraw */
    with_view_model(app->passthru_view, PassthruModel* _model,
			{UNUSED(_model);}, true);

    passthru_model->update_display = false;
  }
}



/** USB serial passthrough view enter callback
    Configure the virtual COM port, start the RX/TX thread and setup the timer
    to update the passthrough view regularly **/
void passthru_view_enter_callback(void *ctx) {

  App *app = (App *)ctx;
  uint32_t period = furi_ms_to_ticks(passthru_view_update_every);

  with_view_model(app->passthru_view, PassthruModel *passthru_model,
	{
	  /* Reset the serial traffic counters */
	  passthru_model->total_bytes_sent = 0;
	  passthru_model->total_bytes_recv = 0;

	  /* Create the virtual COM port TX semaphore, to avoid sending data
	     before the previous transmission is finished */
	  passthru_model->vcp_tx_sem = furi_semaphore_alloc(1, 1);

	  /* Start out assuming the virtual COM port isn't connected and nothing
	     has opened it on the remote end yet */
	  passthru_model->vcp_connected = false;
	  passthru_model->vcp_open = false;

	  /* Get the current virtual COM port configuration */
	  passthru_model->vcp_config =
			furi_hal_cdc_get_port_settings(passthru_vcp_channel);

	  /* Mirror the virtual COM port on the UART */
	  mirror_vcp_on_uart(app, passthru_model);

	  /* Start with the passthrough enabled */
	  passthru_model->enabled = true;

	  furi_hal_usb_unlock();

	  /* Are we supposed to use channel 0 */
	  if(passthru_vcp_channel == 0) {

	    /* Close the CLI */
	    Cli *cli = furi_record_open(RECORD_CLI);
	    cli_session_close(cli);
	    furi_record_close(RECORD_CLI);

	    /* Make sure the USB CDC is configured as single channel */
	    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
	  }

	  /* We're supposed to use channel 1 */
	  else {

	    /* Make sure the USB CDC is configured as dual channel */
	    furi_check(furi_hal_usb_set_config(&usb_cdc_dual, NULL) == true);
	  }

	  /* Allocate space for the virtual COM port RX/TX thread */
	  passthru_model->vcp_rx_tx_thread = furi_thread_alloc();

	  /* Initialize the virtual COM port RX/TX thread */
	  furi_thread_set_name(passthru_model->vcp_rx_tx_thread, "vcp_rx");
	  furi_thread_set_stack_size(passthru_model->vcp_rx_tx_thread, 1024);
	  furi_thread_set_context(passthru_model->vcp_rx_tx_thread, app);
	  furi_thread_set_callback(passthru_model->vcp_rx_tx_thread,
					vcp_rx_tx_thread);

	  /* Start the virtual COM port RX/TX thread */
	  furi_thread_start(passthru_model->vcp_rx_tx_thread);

	  /* Set the virtual COM port callbacks */
	  furi_hal_cdc_set_callbacks(passthru_vcp_channel, &cdc_callbacks, app);

	  /* Update the display for the first time */
	  passthru_model->update_display = true;
	},
	false);

  /* Setup and start the view update timer */
  app->passthru_view_timer = furi_timer_alloc(passthru_view_timer_callback,
						FuriTimerTypePeriodic, ctx);
  furi_timer_start(app->passthru_view_timer, period);
}



/** USB serial passthrough view exit callback
    Stop the timer to update the passthrough view regularly, stop the virtual
    COM port RX/TX thread and release the virtual COM port **/
void passthru_view_exit_callback(void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  UNUSED(passthru_model);

  /* If the UART is started, unset the callback to receive raw LRF data and
     stop the UART */
  if(passthru_model->uart_baudrate) {
    set_lrf_raw_data_handler(app->lrf_serial_comm_app, NULL, app);
    stop_uart(app->lrf_serial_comm_app);
  }

  /* Unset the virtual COM port callbacks */
  furi_hal_cdc_set_callbacks(passthru_vcp_channel, NULL, NULL);

  /* Stop and free the virtual COM port RX/TX thread */
  furi_thread_flags_set(furi_thread_get_id(passthru_model->vcp_rx_tx_thread),
						stop);
  furi_thread_join(passthru_model->vcp_rx_tx_thread);
  furi_thread_free(passthru_model->vcp_rx_tx_thread);

  /* Were we using channel 0? */
  if(passthru_vcp_channel == 0) {

    /* Restart the CLI on channel 0 */
    Cli *cli = furi_record_open(RECORD_CLI);
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);
  }

  /* We were using channel 1 */
  else {

    /* Reconfigure the USB CDC as single channel */
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
  }

  /* Stop and free the view update timer */
  furi_timer_stop(app->passthru_view_timer);
  furi_timer_free(app->passthru_view_timer);

  /* Free the virtual COM port TX semaphore */
  furi_semaphore_free(passthru_model->vcp_tx_sem);
}



/** Draw callback for the USB serial passthrough view **/
void passthru_view_draw_callback(Canvas *canvas, void *model) {

  PassthruModel *passthru_model = (PassthruModel *)model;

  /* First print all the things we need to print in the FontBigNumber font */
  canvas_set_font(canvas, FontBigNumbers);

  /* Print the number of bytes sent to the LRF */
  snprintf(passthru_model->spstr, sizeof(passthru_model->spstr),
		"%ld", passthru_model->total_bytes_sent);
  canvas_draw_str_aligned(canvas, 64, 14, AlignCenter, AlignBottom,
				passthru_model->spstr);

  /* Print the number of bytes received from the LRF */
  snprintf(passthru_model->spstr, sizeof(passthru_model->spstr),
		"%ld", passthru_model->total_bytes_recv);
  canvas_draw_str_aligned(canvas, 64, 46, AlignCenter, AlignBottom,
				passthru_model->spstr);

  /* Draw the icon to show the directions of serial traffic in-between */
  canvas_draw_icon(canvas, 5, 16, &I_flipper_lrf_serial_traffic);

  /* Draw a dividing line between the serial traffic stats  and the bottom
     line */
  canvas_draw_line(canvas, 0, 48, 128, 48);

  /* Show the connected state of the virtual COM port using an icon at the
     bottom left */
  canvas_draw_icon(canvas, 0, 54, passthru_model->vcp_connected?
						&I_connected : &I_disconnected);

  /* Show whether something has opened the virtual COM port at the remote end
     using an icon next to the connected icon if the port is connected */
  canvas_draw_icon(canvas, 20, 52, passthru_model->vcp_connected &&
					passthru_model->vcp_open?
						&I_open : &I_closed);

  /* Finally print the bottom line information in the FontPrimary font
     (bold, proportional) */
  canvas_set_font(canvas, FontPrimary);

  /* Print the UART baudrate next to the open icon if the virtual COM port has
     been opened and we have a baudrate to display */
  if(passthru_model->vcp_connected && passthru_model->vcp_open &&
	passthru_model->uart_baudrate_name) {
    canvas_draw_str(canvas, 31, 62, passthru_model->uart_baudrate_name);
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
bool passthru_view_input_callback(InputEvent *evt, void *ctx) {

  App *app = (App *)ctx;
  PassthruModel *passthru_model = view_get_model(app->passthru_view);

  /* If the user pressed the OK button, start or stop the passthrough */
  if(evt->type == InputTypePress && evt->key == InputKeyOk) {

    FURI_LOG_D(TAG, "OK button pressed");

    passthru_model->enabled = !passthru_model->enabled;

    /* Mirror the virtual COM port on the UART */
    mirror_vcp_on_uart(app, passthru_model);

    /* Update the display */
    passthru_model->update_display = true;

    return true;
  }

  /* We haven't handled this event */
  return false;
}
