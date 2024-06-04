/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * HC11 radio communication app
***/

#pragma once

/*** Defines ***/
#define CC1101_MAX_PKT_SIZE 256
#define RX_BUF_SIZE 1024
#define TX_BUF_SIZE 1024

/*** Parameters ***/
extern const uint32_t baudrate_values[];
extern const char* baudrate_names[];
extern const uint8_t nb_baudrate_values;

/*** Types ***/

/** App structure **/
struct _HC11RadioCommApp;
typedef struct _HC11RadioCommApp HC11RadioCommApp;

/*** Routines ***/

/** Set the callback to handle received raw data **/
void set_raw_data_handler(HC11RadioCommApp*, void (*)(uint8_t*, uint16_t, void*), void*);

/** Initialize the HC11 radio communication app **/
HC11RadioCommApp* hc11_radio_comm_app_init(uint16_t);

/** Start the radio **/
void start_radio(HC11RadioCommApp*, uint8_t, uint8_t, uint8_t, uint8_t);

/** Stop the radio **/
void stop_radio(HC11RadioCommApp*);

/** Send function **/
void radio_tx(HC11RadioCommApp*, uint8_t*, uint16_t);

/** Stop the receive thread and free up the space allocated for the HC11 radio
    communication app **/
void hc11_radio_comm_app_free(HC11RadioCommApp*);
