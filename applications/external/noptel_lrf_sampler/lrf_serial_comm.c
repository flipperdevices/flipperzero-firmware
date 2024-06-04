/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * LRF Serial communication app
***/

/*** Includes ***/
#include <furi_hal.h>
#include <expansion/expansion.h>

#include "lrf_serial_comm.h"
#include "led_control.h"

/*** Defines ***/
#define TAG "lrf_serial_comm"

#define UART_RX_STREAM_BUF_SIZE 1024

#define CR 13
#define LF 10
#define SLASH 47

/*** Parameters ***/

/** Prebuilt LRF Commands **/
static uint8_t cmd_smm[] = "\xcc\x00\x00\x00\x9c";
static uint8_t cmd_cmm_1hz[] = "\xcc\x01\x00\x00\x9d";
static uint8_t cmd_cmm_4hz[] = "\xcc\x02\x00\x00\x9e";
static uint8_t cmd_cmm_10hz[] = "\xcc\x03\x00\x00\x9f";
static uint8_t cmd_cmm_20hz[] = "\xcc\x04\x00\x00\x80";
static uint8_t cmd_cmm_100hz[] = "\xcc\x05\x00\x00\x81";
static uint8_t cmd_cmm_200hz[] = "\xcc\x06\x00\x00\x82";
static uint8_t cmd_cmm_break[] = "\xc6\x96";
static uint8_t cmd_pointer_on[] = "\xc5\x02\x97";
static uint8_t cmd_pointer_off[] = "\xc5\x00\x95";
static uint8_t cmd_send_ident[] = "\xc0\x90";
static uint8_t cmd_send_info[] = "\xc2\x92";
static uint8_t cmd_read_diag[] = "\xdc\x8c";

static uint8_t* lrf_cmds[] = {
    cmd_smm, /* smm */
    cmd_cmm_1hz, /* cmm_1hz */
    cmd_cmm_4hz, /* cmm_4hz */
    cmd_cmm_10hz, /* cmm_10hz */
    cmd_cmm_20hz, /* cmm_20hz */
    cmd_cmm_100hz, /* cmm_100hz */
    cmd_cmm_200hz, /* cmm_200hz */
    cmd_cmm_break, /* cmm_break */
    cmd_pointer_on, /* pointer_on */
    cmd_pointer_off, /* pointer_off */
    cmd_send_ident, /* send_ident */
    cmd_send_info, /* send_info */
    cmd_read_diag, /* read_diag */
};

static const uint8_t lrf_cmds_len[] = {
    sizeof(cmd_smm), /* smm */
    sizeof(cmd_cmm_1hz), /* cmm_1hz */
    sizeof(cmd_cmm_4hz), /* cmm_4hz */
    sizeof(cmd_cmm_10hz), /* cmm_10hz */
    sizeof(cmd_cmm_20hz), /* cmm_20hz */
    sizeof(cmd_cmm_100hz), /* cmm_100hz */
    sizeof(cmd_cmm_200hz), /* cmm_200hz */
    sizeof(cmd_cmm_break), /* cmm_break */
    sizeof(cmd_pointer_on), /* pointer_on */
    sizeof(cmd_pointer_off), /* pointer_off */
    sizeof(cmd_send_ident), /* send_ident */
    sizeof(cmd_send_info), /* send_info */
    sizeof(cmd_read_diag), /* read_diag */
};

static const char* lrf_cmds_desc[] = {
    "SMM command", /* smm */
    "Start CMM at 1Hz", /* cmm_1hz */
    "Start CMM at 4Hz", /* cmm_4hz */
    "Start CMM at 10Hz", /* cmm_10hz */
    "Start CMM at 20Hz", /* cmm_20hz */
    "Start CMM at 100Hz", /* cmm_100hz */
    "Start CMM at 200Hz", /* cmm_200hz */
    "CMM break", /* cmm_break */
    "Pointer ON", /* pointer_on */
    "Pointer OFF", /* pointer_off */
    "Send identification frame", /* send_ident */
    "Send infornation frame", /* send_info */
    "Read diagnostic data", /* read_diag */
};

/*** Types ***/

/** App structure **/
struct _LRFSerialCommApp {
    /* Shared storage space and size */
    uint8_t* shared_storage;
    uint16_t shared_storage_size;

    /* Whether the UART is initialized */
    bool is_uart_initialized;

    /* UART receive thread */
    FuriThread* rx_thread;

    /* UART receive stream buffer */
    FuriStreamBuffer* rx_stream;

    /* UART receive timeout */
    uint16_t uart_rx_timeout;

    /* Receive buffer */
    uint8_t rx_buf[UART_RX_BUF_SIZE];

    /* Default LRF frame decode buffer */
    uint8_t default_dec_buf[128];

    /* Actual LRF frame decode buffer */
    uint8_t* dec_buf;
    uint16_t nb_dec_buf;
    uint16_t dec_buf_size;

    /* Callback to send raw received data to and the context we should pass it */
    void (*lrf_raw_data_handler)(uint8_t*, uint16_t, void*);
    void* lrf_raw_data_handler_ctx;

    /* Callback to send a decoded LRF sample to and the context we should
     pass it */
    void (*lrf_sample_handler)(LRFSample*, void*);
    void* lrf_sample_handler_ctx;

    /* Callback to send a decoded LRF identification frame to and the context
     we should pass it */
    void (*lrf_ident_handler)(LRFIdent*, void*);
    void* lrf_ident_handler_ctx;

    /* Callback to send a decoded LRF information frame to and the context
     we should pass it */
    void (*lrf_info_handler)(LRFInfo*, void*);
    void* lrf_info_handler_ctx;

    /* Callback to send diagnostic data to and the context we should pass it */
    void (*diag_data_handler)(LRFDiag*, void*);
    void* diag_data_handler_ctx;

    /* UART channel and handle */
    FuriHalSerialId serial_channel;
    FuriHalSerialHandle* serial_handle;

    /* LED control */
    LEDControl led_control;
};

/** Receive thread events **/
typedef enum {
    stop = 1,
    rx_done = 2,
} rx_thread_evts;

/*** Routines ***/

/** Set the callback to handle raw data received from the LRF **/
void set_lrf_raw_data_handler(
    LRFSerialCommApp* app,
    void (*cb)(uint8_t*, uint16_t, void*),
    void* ctx) {
    app->lrf_raw_data_handler = cb;
    app->lrf_raw_data_handler_ctx = ctx;
}

/** Set the callback to handle one received LRF sample **/
void set_lrf_sample_handler(LRFSerialCommApp* app, void (*cb)(LRFSample*, void*), void* ctx) {
    app->lrf_sample_handler = cb;
    app->lrf_sample_handler_ctx = ctx;
}

/** Set the callback to handle one received LRF identification frame **/
void set_lrf_ident_handler(LRFSerialCommApp* app, void (*cb)(LRFIdent*, void*), void* ctx) {
    app->lrf_ident_handler = cb;
    app->lrf_ident_handler_ctx = ctx;
}

/** Set the callback to handle one received LRF information frame **/
void set_lrf_info_handler(LRFSerialCommApp* app, void (*cb)(LRFInfo*, void*), void* ctx) {
    app->lrf_info_handler = cb;
    app->lrf_info_handler_ctx = ctx;
}

/** Set the callback to handle received diagnostic data **/
void set_diag_data_handler(LRFSerialCommApp* app, void (*cb)(LRFDiag*, void*), void* ctx) {
    app->diag_data_handler = cb;
    app->diag_data_handler_ctx = ctx;
}

/** Enable or disable the use of the shared storage space as LRF frame decode
    buffer **/
void enable_shared_storage_dec_buf(LRFSerialCommApp* app, bool enabled) {
    /* Switch the decode buffer pointer and size as needed */
    if(enabled) {
        app->dec_buf = app->shared_storage;
        app->dec_buf_size = app->shared_storage_size;
    } else {
        app->dec_buf = app->default_dec_buf;
        app->dec_buf_size = sizeof(app->default_dec_buf);
    }

    /* Reset the decode buffer */
    app->nb_dec_buf = 0;
}

/** IRQ callback **/
static void on_uart_irq_callback(FuriHalSerialHandle* hndl, FuriHalSerialRxEvent evt, void* ctx) {
    LRFSerialCommApp* app = (LRFSerialCommApp*)ctx;

    if(evt == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(hndl);
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->rx_thread), rx_done);
    }
}

/** Copy bytes to a string and stop as soon as a non-printable character or
    space is encountered */
void strcpy_rstrip(char* dst, uint8_t* src) {
    uint8_t i;

    for(i = 0; src[i] > 32 && src[i] < 127; i++) dst[i] = src[i];
    dst[i] = 0;
}

/** LRF frame check byte calculator **/
static uint8_t checkbyte(uint8_t* data, uint16_t len) {
    uint8_t checksum = 0;
    uint16_t i;

    for(i = 0; i < len; i++) checksum += data[i];

    checksum ^= 0x50;

    return checksum;
}

/** Time difference in milliseconds between system ticks in milliseconds,
    taking the timestamp overflow into account **/
static uint32_t ms_tick_time_diff_ms(uint32_t tstamp1, uint32_t tstamp2) {
    if(tstamp1 >= tstamp2)
        return tstamp1 - tstamp2;

    else
        return 0xffffffff - tstamp2 + 1 + tstamp1;
}

/** UART receive thread **/
static int32_t uart_rx_thread(void* ctx) {
    LRFSerialCommApp* app = (LRFSerialCommApp*)ctx;
    uint32_t evts;
    uint32_t last_rx_tstamp_ms = 0, now_ms;
    uint16_t rx_buf_len;
    uint32_t wait_nb_dec_buf = 0;
    bool is_little_endian;
    LRFSample lrf_sample;
    LRFIdent lrf_ident;
    LRFInfo lrf_info;
    uint8_t electronics;
    uint8_t fw_major, fw_minor, fw_micro, fw_build;
    LRFDiag lrf_diag = {NULL, 0, 0};
    uint32_t last_update_diag_handler = 0;
    uint16_t i, j;

    /* Union to convert bytes to float, initialized with the endianness test value
     of 1234.0 */
    union {
        uint8_t bytes[4];
        float val;
    } float_union = {.bytes = {0x00, 0x40, 0x9a, 0x44}};

    /* Union to convert bytes to uint16_t or int16_t */
    union {
        uint8_t bytes[2];
        uint16_t unsigned_val;
        int16_t signed_val;
    } int16_union;

    /* Test endianness */
    is_little_endian = float_union.val == 1234.0;

/* Endian-independent value decoding macros */
#define LE2LE_FLOAT_AT_OFFSET(offset)                \
    float_union.bytes[0] = app->dec_buf[offset];     \
    float_union.bytes[1] = app->dec_buf[offset + 1]; \
    float_union.bytes[2] = app->dec_buf[offset + 2]; \
    float_union.bytes[3] = app->dec_buf[offset + 3];

#define LE2BE_FLOAT_AT_OFFSET(offset)                \
    float_union.bytes[3] = app->dec_buf[offset];     \
    float_union.bytes[2] = app->dec_buf[offset + 1]; \
    float_union.bytes[1] = app->dec_buf[offset + 2]; \
    float_union.bytes[0] = app->dec_buf[offset + 3];

#define LE2LE_INT16_AT_OFFSET(offset)            \
    int16_union.bytes[0] = app->dec_buf[offset]; \
    int16_union.bytes[1] = app->dec_buf[offset + 1];

#define LE2BE_INT16_AT_OFFSET(offset)            \
    int16_union.bytes[1] = app->dec_buf[offset]; \
    int16_union.bytes[0] = app->dec_buf[offset + 1];

    while(1) {
        /* Get events */
        evts = furi_thread_flags_wait(stop | rx_done, FuriFlagWaitAny, FuriWaitForever);

        /* Check for errors */
        furi_check((evts & FuriFlagError) == 0);

        /* Should we stop the thread? */
        if(evts & stop) break;

        /* Have we received data? */
        if(evts & rx_done) {
            /* Get the data */
            rx_buf_len =
                furi_stream_buffer_receive(app->rx_stream, app->rx_buf, UART_RX_BUF_SIZE, 0);

            /* Did we actually get something? */
            if(rx_buf_len > 0) {
                /* Start a green LED flash */
                start_led_flash(&app->led_control, GREEN);

                /* Get the current timestamp */
                now_ms = furi_get_tick();

                /* If too much time has passed since the previous data was received,
           reset the decode buffer */
                if(app->nb_dec_buf &&
                   ms_tick_time_diff_ms(now_ms, last_rx_tstamp_ms) >= app->uart_rx_timeout) {
                    FURI_LOG_T(TAG, "RX timeout");
                    app->nb_dec_buf = 0;
                }

                last_rx_tstamp_ms = now_ms;

                /* If we have a callback to handle raw LRF data, call it, pass it the
           data, and don't do any further processing */
                if(app->lrf_raw_data_handler) {
                    app->lrf_raw_data_handler(
                        app->rx_buf, rx_buf_len, app->lrf_raw_data_handler_ctx);
                    continue;
                }

                /* Process the data we're received */
                for(i = 0; i < rx_buf_len; i++) {
                    switch(app->nb_dec_buf) {
                    /* We're waiting for a sync byte */
                    case 0:
                        if(app->rx_buf[i] == 0x59)
                            app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                        break;

                    /* We're waiting for a command byte */
                    case 1:

                        /* What command byte did we get? */
                        switch(app->rx_buf[i]) {
                        /* We got a range measurement response */
                        case 0xcc:
                            app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                            wait_nb_dec_buf = 22; /* We need to get 22 bytes total
					   for this frame */
                            break;

                        /* We got an identification frame response */
                        case 0xc0:
                            app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                            wait_nb_dec_buf = 73; /* We need to get 73 bytes total
					   for this frame */
                            break;

                        /* We got an information frame response */
                        case 0xc2:
                            app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                            wait_nb_dec_buf = 40; /* We need to get 40 bytes total
					   for this frame */
                            break;

                        /* We got a read diagnostic data response */
                        case 0xdc:
                            app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                            wait_nb_dec_buf = 6; /* We need to get 4 more bytes to know
					   how many we need to get in total */
                            break;

                        /* We got an unknown command byte: reset the decode buffer */
                        default:
                            app->nb_dec_buf = 0;
                        }
                        break;

                    /* We're decoding a command */
                    default:

                        /* Add the byte to the decode buffer, making sure the it doesn't
                 overflow */
                        app->dec_buf[app->nb_dec_buf++] = app->rx_buf[i];
                        if(app->nb_dec_buf >= app->dec_buf_size) app->nb_dec_buf--;

                        /* Do we still not have all the expected data? */
                        if(app->nb_dec_buf < wait_nb_dec_buf) {
                            /* If we have a diagnostic data handler, we're receiving the
                   bulk of a diagnostic data frame, we have an even number of
                   bytes and we're due to send an update update on the progress
                   of the download to the diagnostic data handler, do so */
                            if(app->diag_data_handler && app->dec_buf[1] == 0xdc &&
                               wait_nb_dec_buf > 6 && !(app->nb_dec_buf & 1) &&
                               ms_tick_time_diff_ms(now_ms, last_update_diag_handler) >
                                   DIAG_PROGRESS_UPDATE_EVERY) {
                                lrf_diag.nb_vals = (app->nb_dec_buf - 2) / 2;
                                app->diag_data_handler(&lrf_diag, app->diag_data_handler_ctx);
                                last_update_diag_handler = now_ms;
                            }

                            /* Continue getting data into the decode buffer */
                            break;
                        }

                        /* If we're receiving diagnostic data and we only have the start
                 of the frame, recalculate the total number of bytes we need
                 to get */
                        if(wait_nb_dec_buf == 6) {
                            /* Decode the data count before the histogram */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(2);
                            } else {
                                LE2BE_INT16_AT_OFFSET(2);
                            }
                            wait_nb_dec_buf += (int16_union.unsigned_val - 1) * 2;

                            /* Decode the histogram length */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(4);
                            } else {
                                LE2BE_INT16_AT_OFFSET(4);
                            }
                            wait_nb_dec_buf += int16_union.unsigned_val * 2;

                            wait_nb_dec_buf++; /* One last byte for the checkbyte */

                            /* If the new number of bytes to get is too low or exceeds the
                   size of the decode buffer, reset the decode buffer */
                            if(wait_nb_dec_buf <= 6 || wait_nb_dec_buf > app->dec_buf_size) {
                                app->nb_dec_buf = 0;
                                break;
                            }

                            /* Initialize the LRF diagnostic data: for now set vals to NULL
                   since the download isn't complete */
                            lrf_diag.vals = NULL;
                            lrf_diag.nb_vals = (app->nb_dec_buf - 2) / 2;
                            lrf_diag.total_vals = (wait_nb_dec_buf - 2 - 1) / 2;

                            /* If we have a diagnostic data handler, inform it of the
                   progress of the download for the first time */
                            if(app->diag_data_handler) {
                                app->diag_data_handler(&lrf_diag, app->diag_data_handler_ctx);
                                last_update_diag_handler = now_ms;
                            }

                            break;
                        }

                        /* We have enough bytes: if the frame's checksum doesn't match,
                 discard the frame */
                        if(app->dec_buf[app->nb_dec_buf - 1] !=
                           checkbyte(app->dec_buf, app->nb_dec_buf - 1)) {
                            app->nb_dec_buf = 0;
                            break;
                        }

                        /* Decode the frame */
                        switch(app->dec_buf[1]) {
                        /* We got a range measurement response */
                        case 0xcc:

                            if(is_little_endian) {
                                /* Decode the 1st distance */
                                LE2LE_FLOAT_AT_OFFSET(2)
                                lrf_sample.dist1 = float_union.val;

                                /* Decode the 1st amplitude */
                                LE2LE_INT16_AT_OFFSET(6);
                                lrf_sample.ampl1 = int16_union.unsigned_val;

                                /* Decode the 2nd distance */
                                LE2LE_FLOAT_AT_OFFSET(8)
                                lrf_sample.dist2 = float_union.val;

                                /* Decode the 2nd amplitude */
                                LE2LE_INT16_AT_OFFSET(12);
                                lrf_sample.ampl2 = int16_union.unsigned_val;

                                /* Decode the 3rd distance */
                                LE2LE_FLOAT_AT_OFFSET(14)
                                lrf_sample.dist3 = float_union.val;

                                /* Decode the 3rd amplitude */
                                LE2LE_INT16_AT_OFFSET(18);
                                lrf_sample.ampl3 = int16_union.unsigned_val;
                            }

                            else {
                                /* Decode the 1st distance */
                                LE2BE_FLOAT_AT_OFFSET(2)
                                lrf_sample.dist1 = float_union.val;

                                /* Decode the 1st amplitude */
                                LE2BE_INT16_AT_OFFSET(6);
                                lrf_sample.ampl1 = int16_union.unsigned_val;

                                /* Decode the 2nd distance */
                                LE2BE_FLOAT_AT_OFFSET(8)
                                lrf_sample.dist2 = float_union.val;

                                /* Decode the 2nd amplitude */
                                LE2BE_INT16_AT_OFFSET(12);
                                lrf_sample.ampl2 = int16_union.unsigned_val;

                                /* Decode the 3rd distance */
                                LE2BE_FLOAT_AT_OFFSET(14)
                                lrf_sample.dist3 = float_union.val;

                                /* Decode the 3rd amplitude */
                                LE2BE_INT16_AT_OFFSET(18);
                                lrf_sample.ampl3 = int16_union.unsigned_val;
                            }

                            /* Timestamp the sample */
                            lrf_sample.tstamp_ms = now_ms;

                            FURI_LOG_T(
                                TAG,
                                "LRF sample received: "
                                "dist1=%f, dist2=%f, dist3=%f, "
                                "ampl1=%d, ampl2=%d, ampl3=%d",
                                (double)lrf_sample.dist1,
                                (double)lrf_sample.dist2,
                                (double)lrf_sample.dist3,
                                lrf_sample.ampl1,
                                lrf_sample.ampl2,
                                lrf_sample.ampl3);

                            /* If we have a callback to handle the decoded LRF sample,
                     call it and pass it the sample */
                            if(app->lrf_sample_handler)
                                app->lrf_sample_handler(&lrf_sample, app->lrf_sample_handler_ctx);

                            break;

                        /* We got an identification frame response */
                        case 0xc0:

                            /* Make sure the LRF ID is terminated by CRLF and discard the
                     frame if it isn't */
                            if(app->dec_buf[17] != CR || app->dec_buf[18] != LF) break;

                            /* Copy the printable left-hand part of the LRF ID */
                            strcpy_rstrip(lrf_ident.id, app->dec_buf + 2);

                            /* Make sure the additional information is terminated by CRLF
                     and discard the frame if it isn't */
                            if(app->dec_buf[34] != CR || app->dec_buf[35] != LF) break;

                            /* Copy the printable left-hand part of the additional
                     information */
                            strcpy_rstrip(lrf_ident.addinfo, app->dec_buf + 19);

                            /* Make sure the serial number is terminated by CRLF
                     and discard the frame if it isn't */
                            if(app->dec_buf[46] != CR || app->dec_buf[47] != LF) break;

                            /* Copy the printable left-hand part of the serial number */
                            strcpy_rstrip(lrf_ident.serial, app->dec_buf + 36);

                            /* Decode the firmware version number */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(48);
                            } else {
                                LE2BE_INT16_AT_OFFSET(48);
                            }

                            /* Get the electronics type */
                            electronics = app->dec_buf[50];

                            /* Get the optics type in readable format */
                            snprintf(lrf_ident.optics, 4, "%d", app->dec_buf[51]);

                            /* Interpret the firmware version information */
                            fw_major = int16_union.unsigned_val >> 12;
                            fw_minor = (int16_union.unsigned_val & 0xf00) >> 8;
                            fw_micro = int16_union.unsigned_val & 0xff;
                            lrf_ident.is_fw_newer_than_x4 = fw_minor > 4 ||
                                                            (fw_minor == 4 && fw_micro > 0);

                            /* Extract the firmware's build number from the electronics
                     type if the firmware is a newer version */
                            if(lrf_ident.is_fw_newer_than_x4) {
                                fw_build = electronics;
                                electronics = 0;
                            } else {
                                fw_build = electronics & 0x0f;
                                electronics >>= 4;
                            }

                            /* Store the eletronics type in readable format */
                            snprintf(lrf_ident.electronics, 4, "%d", electronics);

                            /* Store the firmware version in readable format */
                            snprintf(
                                lrf_ident.fwversion,
                                16,
                                "%d.%d.%d.%d",
                                fw_major,
                                fw_minor,
                                fw_micro,
                                fw_build);

                            /* Make sure the year, month and day of the build date are
                     ASCII digits and discard the frame if they aren't */
                            if(app->dec_buf[52] < 0x30 || app->dec_buf[52] > 0x39 ||
                               app->dec_buf[53] < 0x30 || app->dec_buf[53] > 0x39 ||
                               app->dec_buf[55] < 0x30 || app->dec_buf[55] > 0x39 ||
                               app->dec_buf[56] < 0x30 || app->dec_buf[56] > 0x39 ||
                               app->dec_buf[58] < 0x30 || app->dec_buf[58] > 0x39 ||
                               app->dec_buf[59] < 0x30 || app->dec_buf[59] > 0x39)
                                break;

                            /* Make sure the date is terminated by CRLF and discard the
                     if it isn't */
                            if(app->dec_buf[60] != CR || app->dec_buf[61] != LF) break;

                            /* Make sure the hour, minute and second of the build date are
                     ASCII digits and discard the frame if they aren't */
                            if(app->dec_buf[62] < 0x30 || app->dec_buf[62] > 0x39 ||
                               app->dec_buf[63] < 0x30 || app->dec_buf[63] > 0x39 ||
                               app->dec_buf[65] < 0x30 || app->dec_buf[65] > 0x39 ||
                               app->dec_buf[66] < 0x30 || app->dec_buf[66] > 0x39 ||
                               app->dec_buf[68] < 0x30 || app->dec_buf[68] > 0x39 ||
                               app->dec_buf[69] < 0x30 || app->dec_buf[69] > 0x39)
                                break;

                            /* Get the build date. If the month separator is "/", swap
                     the day and the year */
                            if(app->dec_buf[57] == SLASH)
                                snprintf(
                                    lrf_ident.builddate,
                                    20,
                                    "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                                    app->dec_buf[58],
                                    app->dec_buf[59],
                                    app->dec_buf[55],
                                    app->dec_buf[56],
                                    app->dec_buf[52],
                                    app->dec_buf[53],
                                    app->dec_buf[62],
                                    app->dec_buf[63],
                                    app->dec_buf[65],
                                    app->dec_buf[66],
                                    app->dec_buf[68],
                                    app->dec_buf[69]);
                            else
                                snprintf(
                                    lrf_ident.builddate,
                                    20,
                                    "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                                    app->dec_buf[52],
                                    app->dec_buf[53],
                                    app->dec_buf[55],
                                    app->dec_buf[56],
                                    app->dec_buf[58],
                                    app->dec_buf[59],
                                    app->dec_buf[62],
                                    app->dec_buf[63],
                                    app->dec_buf[65],
                                    app->dec_buf[66],
                                    app->dec_buf[68],
                                    app->dec_buf[69]);

                            FURI_LOG_T(
                                TAG,
                                "LRF identification frame received: "
                                "lrfid=%s, addinfo=%s, serial=%s, "
                                "fwversion=%s, electronics=%s, "
                                "optics=%s, builddate=%s",
                                lrf_ident.id,
                                lrf_ident.addinfo,
                                lrf_ident.serial,
                                lrf_ident.fwversion,
                                lrf_ident.electronics,
                                lrf_ident.optics,
                                lrf_ident.builddate);

                            /* If we have a callback to handle the decoded LRF
                     identification frame, call it and pass it the
                     identification */
                            if(app->lrf_ident_handler)
                                app->lrf_ident_handler(&lrf_ident, app->lrf_ident_handler_ctx);

                            break;

                        /* We got an information frame response */
                        case 0xc2:

                            /* Get the number of transmission retries */
                            lrf_info.txretries = app->dec_buf[2];

                            /* Decode the laser pump time */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(3)
                            } else {
                                LE2BE_INT16_AT_OFFSET(3)
                            }
                            lrf_info.txpumptime = int16_union.unsigned_val;

                            /* Decode the number of pulses used in the last measurement */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(5)
                            } else {
                                LE2BE_INT16_AT_OFFSET(5)
                            }
                            lrf_info.pulsesused = int16_union.unsigned_val;

                            /* Decode the transmitter temperature */
                            lrf_info.txtemp = app->dec_buf[7];

                            /* Get the APD at first burst */
                            lrf_info.apdatfirstburst = app->dec_buf[8];

                            /* Get the 1st target distance */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(10)
                            } else {
                                LE2BE_INT16_AT_OFFSET(10)
                            }
                            lrf_info.targetdist1 = int16_union.unsigned_val;

                            /* Get the 2nd target distance */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(12)
                            } else {
                                LE2BE_INT16_AT_OFFSET(12)
                            }
                            lrf_info.targetdist2 = int16_union.unsigned_val;

                            /* Get the 3rd target distance */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(14)
                            } else {
                                LE2BE_INT16_AT_OFFSET(14)
                            }
                            lrf_info.targetdist3 = int16_union.unsigned_val;

                            /* Get the 1st target magnitude */
                            lrf_info.targetmagnitude1 = app->dec_buf[16];

                            /* Get the 2nd target magnitude */
                            lrf_info.targetmagnitude2 = app->dec_buf[17];

                            /* Get the 3rd target magnitude */
                            lrf_info.targetmagnitude3 = app->dec_buf[18];

                            /* Decode the battery voltage */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(20)
                            } else {
                                LE2BE_INT16_AT_OFFSET(20)
                            }
                            lrf_info.battvoltage = (float)int16_union.unsigned_val * 0.001;

                            /* Decode the I/O voltage */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(24)
                            } else {
                                LE2BE_INT16_AT_OFFSET(24)
                            }
                            lrf_info.iovoltage = (float)(int16_union.unsigned_val - 3300) * 0.001;

                            /* Decode the receiver voltage */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(26)
                            } else {
                                LE2BE_INT16_AT_OFFSET(26)
                            }
                            lrf_info.rxvoltage = (float)int16_union.unsigned_val * 0.01;

                            /* Decode the transmitter voltage */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(28)
                            } else {
                                LE2BE_INT16_AT_OFFSET(28)
                            }
                            lrf_info.txvoltage = (float)int16_union.unsigned_val * 0.001;

                            /* Decode the receiver temperature */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(30)
                            } else {
                                LE2BE_INT16_AT_OFFSET(30)
                            }
                            lrf_info.rxtemp = (float)int16_union.signed_val * 0.01;

                            /* Get status bytes */
                            lrf_info.statusbyte1 = app->dec_buf[32];
                            lrf_info.statusbyte2 = app->dec_buf[33];
                            lrf_info.statusbyte3 = app->dec_buf[34];

                            /* Decode the pulse counter */
                            if(is_little_endian) {
                                LE2LE_INT16_AT_OFFSET(35)
                            } else {
                                LE2BE_INT16_AT_OFFSET(35)
                            }
                            lrf_info.pulsectr =
                                (int16_union.unsigned_val + (app->dec_buf[37] << 16)) * 1e6;

                            /* Get the serial error counter */
                            lrf_info.rserrorctr = app->dec_buf[38];

                            FURI_LOG_T(
                                TAG,
                                "LRF information frame received: "
                                "txretries=%d, txpumptime=%d, "
                                "pulsesused=%d, txtemp=%d, "
                                "apdatfirstburst=%d, targetdist1=%d, "
                                "targetdist2=%d, targetdist3=%d, "
                                "targetmagnitude1=%d, "
                                "targetmagnitude2=%d, "
                                "targetmagnitude3=%d, "
                                "battvoltage=%0.3f, iovoltage=%0.3f, "
                                "rxvoltage=%0.2f, txvoltage=%0.3f, "
                                "rxtemp=%0.2f, statusbyte1=%02x, "
                                "statusbyte2=%02x, statusbyte3=%02x, "
                                "pulsectr=%lld, rserrorctr=%d",
                                lrf_info.txretries,
                                lrf_info.txpumptime,
                                lrf_info.pulsesused,
                                lrf_info.txtemp,
                                lrf_info.apdatfirstburst,
                                lrf_info.targetdist1,
                                lrf_info.targetdist2,
                                lrf_info.targetdist3,
                                lrf_info.targetmagnitude1,
                                lrf_info.targetmagnitude2,
                                lrf_info.targetmagnitude3,
                                (double)lrf_info.battvoltage,
                                (double)lrf_info.iovoltage,
                                (double)lrf_info.rxvoltage,
                                (double)lrf_info.txvoltage,
                                (double)lrf_info.rxtemp,
                                lrf_info.statusbyte1,
                                lrf_info.statusbyte2,
                                lrf_info.statusbyte3,
                                lrf_info.pulsectr,
                                lrf_info.rserrorctr);

                            /* If we have a callback to handle the decoded LRF information
                     frame, call it and pass it the information */
                            if(app->lrf_info_handler)
                                app->lrf_info_handler(&lrf_info, app->lrf_info_handler_ctx);

                            break;

                        /* We got a read diagnostic data response */
                        case 0xdc:

                            /* Point the diagnostic values to the decode buffer */
                            lrf_diag.vals = (uint16_t*)(app->dec_buf + 2);

                            /* Update the number of values received */
                            lrf_diag.nb_vals = lrf_diag.total_vals;

                            /* Fix up the diagnostic values' endianness if needed */
                            if(!is_little_endian) {
                                for(j = 0; j < lrf_diag.nb_vals; j++)
                                    lrf_diag.vals[j] = (lrf_diag.vals[j] & 0xff00) >> 8 |
                                                       (lrf_diag.vals[j] & 0xff) << 8;
                            }

                            FURI_LOG_T(
                                TAG,
                                "LRF diagnostic data received: %d bytes / "
                                "%d diagnostic values",
                                app->nb_dec_buf,
                                lrf_diag.total_vals);

                            /* If we have a diagnostic data handler, update the it one
                     last time, this time passing it the actual diagnostic
                     values to save */
                            if(app->diag_data_handler)
                                app->diag_data_handler(&lrf_diag, app->diag_data_handler_ctx);

                            break;
                        }

                        /* Clear the decode buffer */
                        app->nb_dec_buf = 0;

                        break;
                    }
                }
            }
        }
    }

    return 0;
}

/** UART send function **/
void uart_tx(LRFSerialCommApp* app, uint8_t* data, uint16_t len) {
    furi_hal_serial_tx(app->serial_handle, data, len);
    furi_hal_serial_tx_wait_complete(app->serial_handle);
}

/** Send a command to the LRF **/
void send_lrf_command(LRFSerialCommApp* app, LRFCommand cmd) {
    /* Start a red LED flash */
    start_led_flash(&app->led_control, RED);

    /* Send the correct sequence of bytes to the LRF depending on the command */
    uart_tx(app, lrf_cmds[cmd], lrf_cmds_len[cmd]);
    FURI_LOG_T(TAG, "%s command sent", lrf_cmds_desc[cmd]);
}

/** Initialize the LRF serial communication app **/
LRFSerialCommApp* lrf_serial_comm_app_init(
    uint16_t min_led_flash_duration,
    uint16_t uart_rx_timeout,
    uint8_t* shared_storage,
    uint16_t shared_storage_size) {
    FURI_LOG_I(TAG, "App init");

    /* Allocate space for the app's structure */
    LRFSerialCommApp* app = malloc(sizeof(LRFSerialCommApp));

    /* The UART isn't initialized yet */
    app->is_uart_initialized = false;

    /* Save the shared storage location and size */
    app->shared_storage = shared_storage;
    app->shared_storage_size = shared_storage_size;

    /* No raw LRF data handler callback setup yet */
    app->lrf_raw_data_handler = NULL;

    /* No received LRF data handler callback setup yet */
    app->lrf_sample_handler = NULL;

    /* No received LRF data identification frame handler callback setup yet */
    app->lrf_ident_handler = NULL;

    /* No received diagnostic data handler callback setup yet */
    app->diag_data_handler = NULL;

    /* Use the default decode buffer to start with */
    enable_shared_storage_dec_buf(app, false);

    /* Allocate space for the UART receive stream buffer */
    app->rx_stream = furi_stream_buffer_alloc(UART_RX_STREAM_BUF_SIZE, 1);

    /* Allocate space for the UART receive thread */
    app->rx_thread = furi_thread_alloc();

    /* Setup the LED control */
    set_led_control(&app->led_control, min_led_flash_duration);

    /* Disable support for expansion modules */
    expansion_disable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);

    /* Initialize the UART receive thread */
    furi_thread_set_name(app->rx_thread, "uart_rx");
    furi_thread_set_stack_size(app->rx_thread, 2048);
    furi_thread_set_context(app->rx_thread, app);
    furi_thread_set_callback(app->rx_thread, uart_rx_thread);

    /* Set the UART receive timeout */
    app->uart_rx_timeout = uart_rx_timeout;

    /* Start the UART receive thread */
    furi_thread_start(app->rx_thread);

    /* Acquire the UART */
    app->serial_handle = furi_hal_serial_control_acquire(app->serial_channel);
    furi_check(app->serial_handle);

    return app;
}

/** Start the UART **/
void start_uart(LRFSerialCommApp* app, uint32_t baudrate) {
    /* If the UART is already initialized, only set the baudrate. Otherwise
     initialize it with the baudrate */
    if(app->is_uart_initialized)
        furi_hal_serial_set_br(app->serial_handle, baudrate);
    else {
        furi_hal_serial_init(app->serial_handle, baudrate);
        app->is_uart_initialized = true;
    }

    /* Start receiving */
    furi_hal_serial_async_rx_start(app->serial_handle, on_uart_irq_callback, app, false);
}

/** Stop the UART **/
void stop_uart(LRFSerialCommApp* app) {
    /* Stop receiving */
    furi_hal_serial_async_rx_stop(app->serial_handle);
}

/** Set the UART's baudrate **/
void set_uart_baudrate(LRFSerialCommApp* app, uint32_t baudrate) {
    furi_hal_serial_set_br(app->serial_handle, baudrate);
}

/** Stop the UART receive thread and free up the space allocated for the LRF
    communication app **/
void lrf_serial_comm_app_free(LRFSerialCommApp* app) {
    FURI_LOG_I(TAG, "App free");

    /* Deinitialize the UART if it's been initialized */
    if(app->is_uart_initialized) furi_hal_serial_deinit(app->serial_handle);

    /* Release the UART */
    furi_hal_serial_control_release(app->serial_handle);

    /* Stop and free the UART receive thread */
    furi_thread_flags_set(furi_thread_get_id(app->rx_thread), stop);
    furi_thread_join(app->rx_thread);
    furi_thread_free(app->rx_thread);

    /* Free the UART receive stream buffer */
    furi_stream_buffer_free(app->rx_stream);

    /* Re-enable support for expansion modules */
    expansion_enable(furi_record_open(RECORD_EXPANSION));
    furi_record_close(RECORD_EXPANSION);

    /* Release the LED control */
    release_led_control(&app->led_control);

    /* Free the LRF serial communication app's structure */
    free(app);
}
