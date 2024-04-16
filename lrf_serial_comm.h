/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.2
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
  smm = 0,
  cmm_1hz = 1,
  cmm_4hz = 4,
  cmm_10hz = 10,
  cmm_20hz = 29,
  cmm_100hz = 100,
  cmm_200hz = 200,
  cmm_break = -1,
  send_ident = -2,
  pointer_on = -3,
  pointer_off = -4,
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
struct _LRFSerialCommApp;
typedef struct _LRFSerialCommApp LRFSerialCommApp;



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
LRFSerialCommApp *lrf_serial_comm_app_init(uint16_t);

/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp *);
