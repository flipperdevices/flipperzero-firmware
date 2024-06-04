/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * LRF Serial communication app
***/

#pragma once

/*** Defines ***/
#define UART_RX_BUF_SIZE 256
#define DIAG_PROGRESS_UPDATE_EVERY 250 /*ms*/

/*** Types ***/

/** LRF command **/
typedef enum {

    /* Trigger one SMM measurement */
    smm = 0,

    /* Start continuous measurement at 1 Hz */
    cmm_1hz = 1,

    /* Start continuous measurement at 4 Hz */
    cmm_4hz = 2,

    /* Start continuous measurement at 10 Hz */
    cmm_10hz = 3,

    /* Start continuous measurement at 20 Hz */
    cmm_20hz = 4,

    /* Start continuous measurement at 100 Hz */
    cmm_100hz = 5,

    /* Start continuous measurement at 200 Hz */
    cmm_200hz = 6,

    /* Stop continuous measurement */
    cmm_break = 7,

    /* Turn the pointer on */
    pointer_on = 8,

    /* Turn the pointer off */
    pointer_off = 9,

    /* Send identification frame */
    send_ident = 10,

    /* Send information frame */
    send_info = 11,

    /* Read diagnostic data */
    read_diag = 12,

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

    /* Whether the firmware is newer than x.4.x */
    bool is_fw_newer_than_x4;

    /* Electronics type */
    char electronics[4];

    /* Optics type */
    char optics[4];

    /* Build date */
    char builddate[32];

} LRFIdent;

/** LRF information **/
typedef struct {
    /* Transmission retries */
    uint8_t txretries;

    /* Laser pump time */
    uint16_t txpumptime;

    /* Number of pulses used for the last measurement */
    uint16_t pulsesused;

    /* Transmitter temperature */
    uint8_t txtemp;

    /* APD at first burst */
    uint8_t apdatfirstburst;

    /* Target distances */
    uint16_t targetdist1;
    uint16_t targetdist2;
    uint16_t targetdist3;

    /* Target magnitudes */
    uint8_t targetmagnitude1;
    uint8_t targetmagnitude2;
    uint8_t targetmagnitude3;

    /* Battery voltage */
    float battvoltage;

    /* I/O voltage */
    float iovoltage;

    /* Receiver voltage */
    float rxvoltage;

    /* Transmitter voltage */
    float txvoltage;

    /* Receiver temperature */
    float rxtemp;

    /* Status bytes */
    uint8_t statusbyte1;
    uint8_t statusbyte2;
    uint8_t statusbyte3;

    /* Pulse counter */
    uint64_t pulsectr;

    /* Serial error counter */
    uint8_t rserrorctr;

} LRFInfo;

/** LRF diagnostic data **/
typedef struct {
    /* Diagnostic data values */
    uint16_t* vals;

    /* Number of values currently read */
    uint16_t nb_vals;

    /* Total number of values */
    uint16_t total_vals;

} LRFDiag;

/** App structure **/
struct _LRFSerialCommApp;
typedef struct _LRFSerialCommApp LRFSerialCommApp;

/*** Routines ***/

/** Set the callback to handle raw data received from the LRF **/
void set_lrf_raw_data_handler(LRFSerialCommApp*, void (*)(uint8_t*, uint16_t, void*), void*);

/** Set the callback to handle one received LRF sample **/
void set_lrf_sample_handler(LRFSerialCommApp*, void (*)(LRFSample*, void*), void*);

/** Set the callback to handle one received LRF identification frame **/
void set_lrf_ident_handler(LRFSerialCommApp*, void (*)(LRFIdent*, void*), void*);

/** Set the callback to handle one received LRF information frame **/
void set_lrf_info_handler(LRFSerialCommApp*, void (*)(LRFInfo*, void*), void*);

/** Set the callback to handle received diagnostic data **/
void set_diag_data_handler(LRFSerialCommApp*, void (*)(LRFDiag*, void*), void*);

/** Enable or disable the use of the shared storage space as LRF frame decode
    buffer **/
void enable_shared_storage_dec_buf(LRFSerialCommApp*, bool);

/** UART send function **/
void uart_tx(LRFSerialCommApp*, uint8_t*, uint16_t);

/** Send a command to the LRF **/
void send_lrf_command(LRFSerialCommApp*, LRFCommand);

/** Initialize the LRF serial communication app **/
LRFSerialCommApp* lrf_serial_comm_app_init(uint16_t, uint16_t, uint8_t*, uint16_t);

/** Start the UART **/
void start_uart(LRFSerialCommApp*, uint32_t);

/** Stop the UART **/
void stop_uart(LRFSerialCommApp*);

/** Set the UART's baudrate **/
void set_uart_baudrate(LRFSerialCommApp*, uint32_t);

/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp*);
