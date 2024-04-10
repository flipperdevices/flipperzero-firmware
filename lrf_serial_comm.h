/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.1
 *
 * LRF Serial communication app
***/

#pragma once

/*** Defines ***/
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
  cmm_break,
  send_ident
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



/** LRF identification **/
typedef struct {

  /* Device ID */
  char id[16];

  /* Additional information */
  char addinfo[16];

  /* Serial number */
  char serial[16];

  /* Firmware version */
  char fwversion[16];

  /* Electronics type */
  char electronics[4];

  /* Optics type */
  char optics[4];

  /* Build date */
  char builddate[32];

} LRFIdent;



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

  /* Callback to send a decoded LRF sample to and the context
     we should pass it */
  void (*lrf_sample_handler)(LRFSample *, void *);
  void *lrf_sample_handler_ctx;

  /* Callback to send a decoded LRF identification frakme to and the context
     we should pass it */
  void (*lrf_ident_handler)(LRFIdent *, void *);
  void *lrf_ident_handler_ctx;

  /* UART channel and handle */
  FuriHalSerialId serial_channel;
  FuriHalSerialHandle *serial_handle;

} LRFSerialCommApp;



/*** Routines ***/

/** Set the callback to handle one received LRF sample **/
void set_lrf_sample_handler(LRFSerialCommApp *, void (*)(LRFSample *, void *),
				void *);

/** Set the callback to handle one received LRF identification frame **/
void set_lrf_ident_handler(LRFSerialCommApp *, void (*)(LRFIdent *, void *),
				void *);

/** Send a command to the LRF **/
void send_lrf_command(LRFSerialCommApp*, LRFCommand);

/** Initialize the LRF serial communication app **/
LRFSerialCommApp *lrf_serial_comm_app_init();

/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp *);
