/*** LRF Serial communication app ***/
#include "lrf_serial_comm.h"



/*** Defines ***/
#define TAG "lrf_serial_comm"



/*** Parameters ***/

/** Prebuilt LRF Commands (exec mode) **/
static uint8_t cmd_smm[] = "\xcc\x00\x00\x00\x9c";
static uint8_t cmd_cmm_1hz[] = "\xcc\x01\x00\x00\x9d";
static uint8_t cmd_cmm_4hz[] = "\xcc\x02\x00\x00\x9e";
static uint8_t cmd_cmm_10hz[] = "\xcc\x03\x00\x00\x9f";
static uint8_t cmd_cmm_20hz[] = "\xcc\x04\x00\x00\x80";
static uint8_t cmd_cmm_100hz[] = "\xcc\x05\x00\x00\x81";
static uint8_t cmd_cmm_200hz[] = "\xcc\x06\x00\x00\x82";
static uint8_t cmd_cmm_break[] = "\xc6\x96";



/*** Types ***/

/** Receive thread events **/
typedef enum {
  stop = 1,
  rx_done = 2,
} rx_thread_evts;



/*** Routines ***/

/** Set the callback to handle the received LRF data **/
void set_lrf_data_handler(LRFSerialCommApp *app,
					void (*cb)(LRFSample *, void *),
					void *ctx) {
  app->lrf_data_handler = cb;
  app->lrf_data_handler_ctx = ctx;
}



/** IRQ callback **/
static void on_uart_irq_callback(FuriHalSerialHandle *hndl,
					FuriHalSerialRxEvent evt, void *ctx) {

  LRFSerialCommApp *app = (LRFSerialCommApp *)ctx;

  if(evt == FuriHalSerialRxEventData) {
    uint8_t data = furi_hal_serial_async_rx(hndl);
    furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
    furi_thread_flags_set(furi_thread_get_id(app->rx_thread), rx_done);
  }
}



/** LRF frame check byte calculator **/
static uint8_t checkbyte(uint8_t *data, uint16_t len) {

  uint8_t checksum = 0;
  int i;

  for(i = 0; i < len; i++)
    checksum += data[i];

  checksum ^= 0x50;

  return checksum;
}



/** UART receive thread **/
static int32_t uart_rx_thread(void *ctx) {

  LRFSerialCommApp *app = (LRFSerialCommApp *)ctx;
  uint32_t evts;
  size_t rx_buf_len;
  uint16_t dec_buf_len = 0;
  uint16_t wait_dec_buf_len;
  bool is_little_endian;
  LRFSample lrf_sample;
  uint16_t i;

  /* Union to convert bytes to float, initialized with the endianness test value
     of 1234.0 */
  union {
    uint8_t bytes[4];
    float f;
  } fun = {.bytes = {0x00, 0x40, 0x9a, 0x44}};

  /* Union to convert bytes to uint16_t */
  union {
    uint8_t bytes[2];
    uint16_t usi;
  } usiun;

  /* Test endianness */
  is_little_endian = fun.f == 1234.0;

  while(1) {

    /* Get until we get either a stop event of we received data */
    evts = furi_thread_flags_wait(stop | rx_done,
					FuriFlagWaitAny, FuriWaitForever);

    /* Check for errors */
    furi_check((evts & FuriFlagError) == 0);

    /* Should we stop the thread? */
    if(evts & stop)
      break;

    /* Have we received data? */
    if(evts & rx_done) {

      /* Get the data */
      rx_buf_len = furi_stream_buffer_receive(app->rx_stream,
						app->rx_buf, RX_BUF_SIZE, 0);

      /* If we actually got something, process the data we're received */
      if(rx_buf_len > 0) {

        for(i=0; i < rx_buf_len; i++)

          switch(dec_buf_len) {

	    /* We're waiting for a sync byte */
            case 0:
              if(app->rx_buf[i] == 0x59)
                app->dec_buf[dec_buf_len++] = app->rx_buf[i];
              break;

	    /* We're waiting for a command byte */
            case 1:

              /* What command byte did we get? */
              switch(app->rx_buf[i]) {

                /* We got an exec range measurement response */
                case 0xcc:
                  app->dec_buf[dec_buf_len++] = app->rx_buf[i];
                  wait_dec_buf_len = 22;	/* We need to get 22 bytes total
						   for this frame */
                  break;

                /* We got an unknown command byte: reset the decode buffer */
                default:
                  dec_buf_len = 0;
              }
              break;

            /* We're decoding a command */
            default:

              /* Keep filling up the decode buffer until we have enough bytes */
              app->dec_buf[dec_buf_len++] = app->rx_buf[i];
              if(dec_buf_len < wait_dec_buf_len)
                break;

              /* We have enough bytes: if the frame's checksum doesn't match,
                 discard the frame */
              if(app->dec_buf[dec_buf_len - 1] !=
				checkbyte(app->dec_buf, dec_buf_len - 1)) {
                dec_buf_len = 0;
                break;
              }

              /* Decode the frame */
              switch(app->dec_buf[1]) {

                /* We got an exec range measurement response */
                case 0xcc:

                  if(is_little_endian) {
                    /* Decode the 1st distance */
                    fun.bytes[0] = app->dec_buf[2];
                    fun.bytes[1] = app->dec_buf[3];
                    fun.bytes[2] = app->dec_buf[4];
                    fun.bytes[3] = app->dec_buf[5];
                    lrf_sample.dist1 = fun.f;

                    /* Decode the 1st amplitude */
                    usiun.bytes[0] = app->dec_buf[6];
                    usiun.bytes[1] = app->dec_buf[7];
                    lrf_sample.ampl1 = usiun.usi;

                    /* Decode the 2nd distance */
                    fun.bytes[0] = app->dec_buf[8];
                    fun.bytes[1] = app->dec_buf[9];
                    fun.bytes[2] = app->dec_buf[10];
                    fun.bytes[3] = app->dec_buf[11];
                    lrf_sample.dist2 = fun.f;

                    /* Decode the 2nd amplitude */
                    usiun.bytes[0] = app->dec_buf[12];
                    usiun.bytes[1] = app->dec_buf[13];
                    lrf_sample.ampl2 = usiun.usi;

                    /* Decode the 3rd distance */
                    fun.bytes[0] = app->dec_buf[14];
                    fun.bytes[1] = app->dec_buf[15];
                    fun.bytes[2] = app->dec_buf[16];
                    fun.bytes[3] = app->dec_buf[17];
                    lrf_sample.dist3 = fun.f;

                    /* Decode the 3rd amplitude */
                    usiun.bytes[0] = app->dec_buf[18];
                    usiun.bytes[1] = app->dec_buf[19];
                    lrf_sample.ampl3 = usiun.usi;
                  }

                  else {
                    /* Decode the 1st distance */
                    fun.bytes[3] = app->dec_buf[2];
                    fun.bytes[2] = app->dec_buf[3];
                    fun.bytes[1] = app->dec_buf[4];
                    fun.bytes[0] = app->dec_buf[5];
                    lrf_sample.dist1 = fun.f;

                    /* Decode the 1st amplitude */
                    usiun.bytes[1] = app->dec_buf[6];
                    usiun.bytes[0] = app->dec_buf[7];
                    lrf_sample.ampl1 = usiun.usi;

                    /* Decode the 2nd distance */
                    fun.bytes[3] = app->dec_buf[8];
                    fun.bytes[2] = app->dec_buf[9];
                    fun.bytes[1] = app->dec_buf[10];
                    fun.bytes[0] = app->dec_buf[11];
                    lrf_sample.dist2 = fun.f;

                    /* Decode the 2nd amplitude */
                    usiun.bytes[1] = app->dec_buf[12];
                    usiun.bytes[0] = app->dec_buf[13];
                    lrf_sample.ampl2 = usiun.usi;

                    /* Decode the 3rd distance */
                    fun.bytes[3] = app->dec_buf[14];
                    fun.bytes[2] = app->dec_buf[15];
                    fun.bytes[1] = app->dec_buf[16];
                    fun.bytes[0] = app->dec_buf[17];
                    lrf_sample.dist3 = fun.f;

                    /* Decode the 3rd amplitude */
                    usiun.bytes[1] = app->dec_buf[18];
                    usiun.bytes[0] = app->dec_buf[19];
                    lrf_sample.ampl3 = usiun.usi;
                  }

                  /* Timestamp the sample */
                  lrf_sample.tstamp_ms = furi_get_tick();

                  /* If we have a callback to handle the decoded LRF data,
                     call it and pass it the data */
                  if(app->lrf_data_handler)
                    app->lrf_data_handler(&lrf_sample,
					app->lrf_data_handler_ctx);

                   FURI_LOG_I(TAG, "LRF sample received: "
					"dist1=%f, dist2=%f, dist3=%f, "
					"ampl1=%d, ampl2=%d, ampl3=%d",
				(double)lrf_sample.dist1,
				(double)lrf_sample.dist2,
				(double)lrf_sample.dist3,
				lrf_sample.ampl1,
				lrf_sample.ampl2,
				lrf_sample.ampl3);

                  break;
              }

              /* Clear the decode buffer */
              dec_buf_len = 0;
           }
      }
    }
  }

  /* Free the UART receive stream buffer */
  furi_stream_buffer_free(app->rx_stream);

  return 0;
}



/** UART send function **/
static void uart_tx(LRFSerialCommApp *app, uint8_t *data, size_t len) {
  furi_hal_serial_tx(app->serial_handle, data, len);
}



/** Send a command to the LRF **/
void send_lrf_command(LRFSerialCommApp *app, LRFCommand cmd) {

  /* Send the correct sequence of bytes to the LRF depending on the command */
  switch(cmd) {

    /* Send an SMM command (exec mode) */
    case smm:
      uart_tx(app, cmd_smm, sizeof(cmd_smm));
      FURI_LOG_I(TAG, "SMM command sent");
      break;

    /* Send a start-CMM command at 1Hz (exec mode) */
    case cmm_1hz:
      uart_tx(app, cmd_cmm_1hz, sizeof(cmd_cmm_1hz));
      FURI_LOG_I(TAG, "Start CMM at 1Hz command sent");
      break;

    /* Send a start-CMM command at 4Hz (exec mode) */
    case cmm_4hz:
      uart_tx(app, cmd_cmm_4hz, sizeof(cmd_cmm_4hz));
      FURI_LOG_I(TAG, "Start CMM at 4Hz command sent");
      break;

    /* Send a start-CMM command at 10Hz (exec mode) */
    case cmm_10hz:
      uart_tx(app, cmd_cmm_10hz, sizeof(cmd_cmm_10hz));
      FURI_LOG_I(TAG, "Start CMM at 10Hz command sent");
      break;

    /* Send a start-CMM command at 20Hz (exec mode) */
    case cmm_20hz:
      uart_tx(app, cmd_cmm_20hz, sizeof(cmd_cmm_20hz));
      FURI_LOG_I(TAG, "Start CMM at 20Hz command sent");
      break;

    /* Send a start-CMM command at 100Hz (exec mode) */
    case cmm_100hz:
      uart_tx(app, cmd_cmm_100hz, sizeof(cmd_cmm_100hz));
      FURI_LOG_I(TAG, "Start CMM at 100Hz command sent");
      break;

    /* Send a start-CMM command at 200Hz (exec mode) */
    case cmm_200hz:
      uart_tx(app, cmd_cmm_200hz, sizeof(cmd_cmm_200hz));
      FURI_LOG_I(TAG, "Start CMM at 200Hz command sent");
      break;

    /* Send a CMM-break command */
    case cmm_break:
      uart_tx(app, cmd_cmm_break, sizeof(cmd_cmm_break));
      break;
  }
}



/** Initialize the LRF serial communication app **/
LRFSerialCommApp *lrf_serial_comm_app_init() {

  FURI_LOG_I(TAG, "App init");

  /* Allocate space for the app's structure */
  LRFSerialCommApp *app = malloc(sizeof(LRFSerialCommApp));

  /* No received LRF data handler callback setup yet */
  app->lrf_data_handler = NULL;

  /* Allocate space for the UART receive stream buffer */
  app->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);

  /* Allocate space for the UART receive thread */
  app->rx_thread = furi_thread_alloc();

  /* Initialize the UART receive thread */
  furi_thread_set_name(app->rx_thread, "LRFSerialCommUARTRxThread");
  furi_thread_set_stack_size(app->rx_thread, 1024);
  furi_thread_set_context(app->rx_thread, app);
  furi_thread_set_callback(app->rx_thread, uart_rx_thread);

  /* Start the UART receive thread */
  furi_thread_start(app->rx_thread);

  /* Get the serial handle */
  app->serial_handle = furi_hal_serial_control_acquire(app->serial_channel);
  furi_check(app->serial_handle);
  furi_hal_serial_init(app->serial_handle, BAUDRATE);
  furi_hal_serial_async_rx_start(app->serial_handle, on_uart_irq_callback,
					app, false);

  return app;
}



/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp *app) {

  FURI_LOG_I(TAG, "App free");

  /* Stop UART receive and release the UART */
  furi_hal_serial_async_rx_stop(app->serial_handle);
  furi_hal_serial_deinit(app->serial_handle);
  furi_hal_serial_control_release(app->serial_handle);

  /* Stop and free the UART receive thread */
  furi_thread_flags_set(furi_thread_get_id(app->rx_thread), stop);
  furi_thread_join(app->rx_thread);
  furi_thread_free(app->rx_thread);

  /* Free the LRF serial communication app's structure */
  free(app);
}
