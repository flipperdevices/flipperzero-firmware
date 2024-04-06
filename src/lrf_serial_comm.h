/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.0
 *
 * LRF Serial communication app
***/

#pragma once

/*** Defines ***/
#define UART FuriHalUartIdUSART1
#define BAUDRATE 115200
#define RX_BUF_SIZE 2048



/*** Types ***/

/** LRF command **/
typedef enum {
  smm,
  cmm_1hz,
  cmm_4hz,
  cmm_10hz,
  cmm_20hz,
  cmm_100hz,
  cmm_200hz,
  cmm_break
} LRFCommand;



/** LRF sample **/
typedef struct {

  /* Distances */
  float dist1;
  float dist2;
  float dist3;

  /* Amplitudes */
  uint16_t ampl1;
  uint16_t ampl2;
  uint16_t ampl3;

  /* Reception timestamp */
  uint32_t tstamp_ms;

} LRFSample;



/** App structure **/
typedef struct {

  /* UART receive thread */
  FuriThread *rx_thread;

  /* UART receive stream buffer */
  FuriStreamBuffer *rx_stream;

  /* Receive buffer */
  uint8_t rx_buf[RX_BUF_SIZE];

  /* LRF frame decoding buffer */
  uint8_t dec_buf[128];

  /* Callback to send decoded LRF data to and the context we should pass it */
  void (*lrf_data_handler)(LRFSample *, void *);
  void *lrf_data_handler_ctx;

  /* UART channel and handle */
  FuriHalSerialId serial_channel;
  FuriHalSerialHandle *serial_handle;

} LRFSerialCommApp;



/*** Routines ***/

/** Set the callback to handle the received LRF data **/
void set_lrf_data_handler(LRFSerialCommApp *, void (*)(LRFSample *, void *),
				void *);

/** Send a command to the LRF **/
void send_lrf_command(LRFSerialCommApp*, LRFCommand);

/** Initialize the LRF serial communication app **/
LRFSerialCommApp *lrf_serial_comm_app_init();

/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp *);
