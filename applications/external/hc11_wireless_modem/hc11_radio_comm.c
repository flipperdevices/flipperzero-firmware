/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * HC11 radio communication app
***/

/*** Includes ***/
#include <furi_hal.h>
#include <furi_hal_subghz.h>
#include <lib/drivers/cc1101_regs.h>

#include "hc11_radio_comm.h"
#include "led_control.h"

/*** Declarations from lib/drivers/cc1101.h

     This is a nasty trick to use the hardware driver directly and bypass the
     HAL. This is inlined here to emphasize how nasty this is, so nobody thinks
     this is good practice in any way.

     This also requires the following line to be added to the application.fam
     to work:

     fap_libs = ["hwdrivers"],
***/
CC1101Status cc1101_strobe(FuriHalSpiBusHandle*, uint8_t);
CC1101Status cc1101_read_reg(FuriHalSpiBusHandle*, uint8_t, uint8_t*);
CC1101Status cc1101_write_reg(FuriHalSpiBusHandle*, uint8_t, uint8_t);

/*** Defines ***/
#define TAG "hc11_radio_comm"

#define RX_STREAM_BUF_SIZE 1024

/*** Parameters ***/
/* Number of "wake up packets" we apparently need to send before sending actual
   data in mode 1 (FU1). This is most likely not the right method, as this works
   very poorly when we do it and the actual HC-11 seems to send only 12 such
   packets and it works fine, but I wasn't able to rev engineer the complete
   correct protocol yet. This is as good as it gets for now. */
const uint8_t mode1_nb_wake_up_pkts = 64;

/* In mode 3 (FU3), the HC-11 apparently sends burts of data in increasingly
   large packets, and the sequence of packet sizes seems to be fixed. This is
   sequence of packet sizes that need to be transmitted from the beginning of
   the transmission until it settles on the final packet size */
const uint8_t mode3_pkt_sizes_seq[] = {3, 34, 47, 52, 55, 56, 57};

/** CC1101 registers preset **/
static uint8_t cc1101_regs[] = {

    /* Channel number */
    CC1101_CHANNR,
    0x02, /* Default channel */

    /* Device address */
    CC1101_ADDR,
    0x00, /* Default address */

    /* GPIO GDO0 */
    CC1101_IOCFG0,
    0x06,

    /* FIFO and internals */
    CC1101_FIFOTHR,
    0x0f,

    /* Packet automation control */
    //    CC1101_PKTCTRL1, 0x4d,
    CC1101_PKTCTRL1,
    0x45, // Don't set CRC_AUTOFLUSH

    /* Packet automation control */
    CC1101_PKTCTRL0,
    0x05,

    /* Frequency synthesizer control */
    CC1101_FSCTRL1,
    0x0c,

    /* Frequency control word, high byte */
    CC1101_FREQ2,
    0x10,

    /* Frequency control word, middle byte */
    CC1101_FREQ1,
    0xb1,

    /* Frequency control word, low byte */
    CC1101_FREQ0,
    0x31,

    /* Modem Configuration */
    CC1101_MDMCFG4,
    0x2d,
    CC1101_MDMCFG3,
    0x3b,
    CC1101_MDMCFG2,
    0x13,

    /* Modem deviation setting */
    CC1101_DEVIATN,
    0x62,

    /* Main radio control state machine configuration */
    CC1101_MCSM1,
    0x30,
    CC1101_MCSM0,
    0x18,

    /* Frequency Offset compensation configuration */
    CC1101_FOCCFG,
    0x1d,

    /* Bit synchronization configuration */
    CC1101_BSCFG,
    0x1c,

    /* Automatic gain control */
    CC1101_AGCCTRL2,
    0x07,
    CC1101_AGCCTRL1,
    0x00,
    CC1101_AGCCTRL0,
    0xb0,

    /* Event 0 timeout */
    CC1101_WOREVT1,
    0x01,
    CC1101_WOREVT0,
    0xf1,

    /* Wake on radio and timeouts control */
    CC1101_WORCTRL,
    0x78,

    /* Frontend configuration */
    CC1101_FREND1,
    0xb6,

    /* Frequency synthesizer calibration */
    CC1101_FSCAL3,
    0xea,
    CC1101_FSCAL2,
    0x2a,
    CC1101_FSCAL1,
    0x00,
    CC1101_FSCAL0,
    0x1f,

    /* Auto-calibration time */
    CC1101_TEST0,
    0x09,

    /* End of regular registers */
    0,
    0,

    /* Default PATable (for power level 4) */
    0x34,
    0x3a,
    0x3b,
    0x36,
    0xf5,
    0x00,
    0x16,
    0x01};

/** PATable[0] for the 8 power levels **/
static const uint8_t tx_power_to_patable_zero[8] = {
    0x12, /* TX power level 1 */
    0x0e, /* TX power level 2 */
    0x1d, /* TX power level 3 */
    0x34, /* TX power level 4 */
    0x60, /* TX power level 5 */
    0x84, /* TX power level 6 */
    0xc8, /* TX power level 7 */
    0xc0 /* TX power level 8 */
};

/*** Types ***/

/** App structure **/
struct _HC11RadioCommApp {
    /* Whether the radio is started */
    bool radio_started;

    /* HC-11 mode */
    uint8_t hc11_mode;

    /* CC1101 address */
    uint8_t cc1101_addr;

    /* Whether the radio is currently receiving */
    bool receiving;

    /* RSSI at the last RX */
    float rssi;

    /* LQI at the last RX */
    uint8_t lqi;

    /* HC11 RX/TX thread */
    FuriThread* hc11_rx_tx_thread;

    /* Main RX thread */
    FuriThread* main_rx_thread;

    /* CC1101 packet receive buffer and packet size */
    uint8_t cc1101_rx_buf_pkt[CC1101_MAX_PKT_SIZE];
    uint8_t cc1101_rx_buf_pkt_size;

    /* CC1101 transmit buffer */
    uint8_t cc1101_tx_buf_pkt[CC1101_MAX_PKT_SIZE];
    uint8_t cc1101_tx_buf_pkt_size;

    /* CC1101 sent semaphore */
    FuriSemaphore* cc1101_sent_sem;

    /* Receive stream buffer */
    FuriStreamBuffer* rx_stream;

    /* Main receive buffer */
    uint8_t rx_buf[RX_BUF_SIZE];

    /* Main transmit buffer, size and  current transmission index */
    uint8_t* tx_buf;
    uint16_t tx_buf_size;
    uint16_t tx_buf_i;

    /* Callback to send raw received data to and the context we should pass it */
    void (*raw_data_handler)(uint8_t*, uint16_t, void*);
    void* raw_data_handler_ctx;

    /* LED control */
    LEDControl led_control;

    /* Scratchpad string */
    char spstr[820];
};

/** Thread events **/
typedef enum {
    stop = 0x01,
    set_cc1101 = 0x02,
    unset_cc1101 = 0x04,
    gdo0_rise = 0x08,
    gdo0_fall = 0x10,
    send_data = 0x20
} thread_evts;

/*** Routines ***/

/** Time difference in milliseconds between system ticks in milliseconds,
    taking the timestamp overflow into account **/
static uint32_t ms_tick_time_diff_ms(uint32_t tstamp1, uint32_t tstamp2) {
    if(tstamp1 >= tstamp2)
        return tstamp1 - tstamp2;

    else
        return 0xffffffff - tstamp2 + 1 + tstamp1;
}

/** Set the callback to handle received raw data **/
void set_raw_data_handler(HC11RadioCommApp* app, void (*cb)(uint8_t*, uint16_t, void*), void* ctx) {
    app->raw_data_handler = cb;
    app->raw_data_handler_ctx = ctx;
}

/** Set CC1101 idle **/
static void cc1101_set_idle(FuriHalSpiBusHandle* spi) {
    uint8_t st;

    cc1101_strobe(spi, CC1101_STROBE_SIDLE);
    do {
        st = cc1101_strobe(spi, CC1101_STROBE_SNOP).STATE;
        if(st != CC1101StateIDLE) cc1101_strobe(spi, CC1101_STROBE_SIDLE);
    } while(st != CC1101StateIDLE);
}

/** Enable CC1101 RX **/
static void cc1101_enable_rx(FuriHalSpiBusHandle* spi) {
    uint8_t st;

    cc1101_strobe(spi, CC1101_STROBE_SRX);
    do {
        st = cc1101_strobe(spi, CC1101_STROBE_SNOP).STATE;
        if(st == CC1101StateIDLE) cc1101_strobe(spi, CC1101_STROBE_SRX);
    } while(st != CC1101StateRX);
}

/** Enable CC1101 TX **/
static void cc1101_enable_tx(FuriHalSpiBusHandle* spi) {
    uint8_t st;

    cc1101_strobe(spi, CC1101_STROBE_STX);
    do {
        st = cc1101_strobe(spi, CC1101_STROBE_SNOP).STATE;
        if(st == CC1101StateIDLE) cc1101_strobe(spi, CC1101_STROBE_STX);
    } while(st != CC1101StateTX);
}

/** CC1101 GDO0 interrupt callback **/
static void cc1101_gdo0_int_callback(void* ctx) {
    HC11RadioCommApp* app = (HC11RadioCommApp*)ctx;

    furi_thread_flags_set(
        furi_thread_get_id(app->hc11_rx_tx_thread),
        furi_hal_gpio_read(&gpio_cc1101_g0) ? gdo0_rise : gdo0_fall);
}

/** HC11 RX/TX thread **/
static int32_t hc11_rx_tx_thread(void* ctx) {
    HC11RadioCommApp* app = (HC11RadioCommApp*)ctx;
    uint8_t loglevel;
    uint32_t evts;
    uint8_t regval_prev[1], regval[1];
    FuriHalSpiBusHandle* spi;
    bool get_pkt_size;
    uint8_t leave_bytes_in_fifo;
    uint8_t pkt_bytes_remaining;
    uint8_t num_rxbytes;
    bool crc_ok;
    uint8_t mode1_wake_up_pkt_ctr;
    uint8_t mode3_pkt_sizes_seq_i;
    bool never_sent_anything;
    uint32_t last_sent_pkt_tstamp;
    uint32_t now_ms;
    uint8_t l, i;

    loglevel = furi_hal_rtc_get_log_level();
    spi = &furi_hal_spi_bus_handle_subghz;

    app->radio_started = false;
    never_sent_anything = true;
    last_sent_pkt_tstamp = 0;

    mode1_wake_up_pkt_ctr = mode1_nb_wake_up_pkts;
    mode3_pkt_sizes_seq_i = 0;

    while(1) {
        /* Get events */
        furi_delay_tick(1);
        evts = furi_thread_flags_wait(
            stop | set_cc1101 | unset_cc1101 | gdo0_rise | gdo0_fall | send_data,
            FuriFlagWaitAny,
            FuriWaitForever);

        /* Check for errors */
        furi_check((evts & FuriFlagError) == 0);

        /* Should we turn off the radio? */
        if((evts & stop || evts & unset_cc1101) && app->radio_started) {
            /* Disable GDO0 interrupts */
            furi_hal_gpio_disable_int_callback(&gpio_cc1101_g0);

            /* Remove the GDO0 interrupt callback */
            furi_hal_gpio_remove_int_callback(&gpio_cc1101_g0);

            /* Reset the GDO line */
            furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

            /* Deinitialize the radio */
            furi_hal_subghz_reset();
            furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);
            furi_hal_subghz_shutdown();

            /* Re-enable charging */
            furi_hal_power_suppress_charge_exit();

            app->radio_started = false;
        }

        /* Should we stop the thread? */
        if(evts & stop) break;

        /* Should we turn on the radio? */
        if((evts & set_cc1101) && !app->radio_started) {
            FURI_LOG_D(TAG, "CC1101 channel: %d", cc1101_regs[1]);
            FURI_LOG_D(TAG, "CC1101 address: %d", cc1101_regs[3]);
            FURI_LOG_D(TAG, "CC1101 patable[0]: 0x%02x", cc1101_regs[sizeof(cc1101_regs) - 8]);

            /* Disable charging */
            furi_hal_power_suppress_charge_enter();

            /* Initialize the radio */
            furi_hal_subghz_load_custom_preset(cc1101_regs);
            furi_hal_subghz_set_path(FuriHalSubGhzPath433);

            /* Configure the GDO0 as an interrupt source */
            furi_hal_gpio_init(
                &gpio_cc1101_g0, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);

            /* Set the GDO0 interrupt callback */
            furi_hal_gpio_add_int_callback(&gpio_cc1101_g0, cc1101_gdo0_int_callback, (void*)app);

            /* Acquire the SPI bus */
            furi_hal_spi_acquire(spi);

            /* Enable RX */
            cc1101_enable_rx(spi);

            /* Release the SPI bus */
            furi_hal_spi_release(spi);

            app->radio_started = true;
            app->receiving = true;
            never_sent_anything = true;
        }

        /* If the radio isn't started, keep getting events */
        if(!app->radio_started) continue;

        /* Have we received a GDO0 rise interrupt and are we receiving? */
        if(evts & gdo0_rise && app->receiving) {
            /* Acquire the SPI bus */
            furi_hal_spi_acquire(spi);

            /* Get the packet */
            app->cc1101_rx_buf_pkt_size = 0;
            get_pkt_size = true;
            leave_bytes_in_fifo = 1;

            while(leave_bytes_in_fifo) {
                /* Get the number of bytes in the RX FIFO.
           Since the CC1101 might still be in the RX state, get the
           status byte until we get the same register value twice
           to work around the CC1101 bug described in
           https://www.ti.com/lit/er/swrz020e/swrz020e.pdf p.6 */
                cc1101_read_reg(spi, CC1101_STATUS_RXBYTES | CC1101_BURST, regval);
                do {
                    regval_prev[0] = regval[0];
                    cc1101_read_reg(spi, CC1101_STATUS_RXBYTES | CC1101_BURST, regval);
                } while(regval_prev[0] != regval[0]);
                num_rxbytes = regval[0] & 0x7f;

                /* Should we get the packet size? */
                if(get_pkt_size) {
                    /* We need at least 2 bytes to get the packet size so we don't empty
            the FIFO prematurely - to work around the CC1101 bug described in
             https://www.ti.com/lit/er/swrz020e/swrz020e.pdf p.2 */
                    if(num_rxbytes < 2) continue;

                    /* Read the packet size (1st byte in the RX FIFO) */
                    cc1101_read_reg(spi, CC1101_FIFO, regval);
                    pkt_bytes_remaining = regval[0];
                    num_rxbytes--;
                    get_pkt_size = false;

                    /* Add 2 bytes for the RSSI and LQI + CRC_OK bytes at the end of
             the packet */
                    pkt_bytes_remaining += 2;
                }

                /* Read as many bytes as possible out of the RX FIFO: if there is
           enough bytes to get the rest of the packet, empty the FIFO.
           Otherwise leave 1 byte in the RX FIFO to avoid emptying it
           prematurely - to work around the CC1101 bug described in
           https://www.ti.com/lit/er/swrz020e/swrz020e.pdf p.2 */
                leave_bytes_in_fifo = num_rxbytes >= pkt_bytes_remaining ? 0 : 1;
                while(pkt_bytes_remaining && num_rxbytes > leave_bytes_in_fifo) {
                    cc1101_read_reg(
                        spi, CC1101_FIFO, app->cc1101_rx_buf_pkt + app->cc1101_rx_buf_pkt_size);
                    app->cc1101_rx_buf_pkt_size++;
                    num_rxbytes--;
                    pkt_bytes_remaining--;
                }
            }

            /* We should never get here with bytes remaining in the RX FIFO. But if
         we do, flush it and warn the user that something is very wrong */
            if(num_rxbytes) {
                while(num_rxbytes--) cc1101_read_reg(spi, CC1101_FIFO, regval);
                if(loglevel == FuriLogLevelTrace)
                    FURI_LOG_T(TAG, "Error: too many bytes in RX FIFO for packet size!");
            }

            /* Get the LQI and CRC_OK flag in the last byte of the packet */
            i = app->cc1101_rx_buf_pkt[--app->cc1101_rx_buf_pkt_size];
            crc_ok = i >> 7;
            app->lqi = i & 0x7f;

            /* Get the RSSI in the penultimate byte of the packet */
            i = app->cc1101_rx_buf_pkt[--app->cc1101_rx_buf_pkt_size];
            app->rssi = (i >= 128 ? i - 256.0f : i) / 2.0f - 74.0f;

            /* Re-enable RX, as the CC1101 is idle as this point since MCSM1 is
         configured to drop the CC1101 to IDLE when it's done receiving the
         packet */
            cc1101_enable_rx(spi);

            /* Release the SPI bus */
            furi_hal_spi_release(spi);

            /* Log the received packet in the traces log. Only do this if the trace
         log level is enabled to save CPU clock to grab packets faster when
         it normally isn't */
            if(loglevel == FuriLogLevelTrace) {
                snprintf(
                    app->spstr,
                    sizeof(app->spstr),
                    "Recv %d bytes - %sRSSI %03.1fdbm, LQI %d: ",
                    app->cc1101_rx_buf_pkt_size,
                    crc_ok ? "" : "CRC NOK!, ",
                    (double)app->rssi,
                    app->lqi);
                l = strlen(app->spstr);
                for(i = 0; i < app->cc1101_rx_buf_pkt_size; i++)
                    snprintf(app->spstr + l + (i * 3), 4, "%02X ", app->cc1101_rx_buf_pkt[i]);
                FURI_LOG_I(TAG, app->spstr);
            }

            /* Decode the packets according to the transmission mode */
            switch(app->hc11_mode) {
            case 1: /* FU1 */

                /* We're only interested in packets that are 56 or 57 bytes long:

             - 56-byte packets contain the address followed by 55 bytes of
               actual data

             - 57-byte packets contain the address followed by x bytes of actual
               data, followed by 55 - x undefined bytes, followed by the x value

             The address should always match since the CC1101 should have
             filtered out packets from senders with another address, but test
             it just in case...

             Also make sure that the x length (last byte) makes sense in a
             57-byte packet */
                l = app->cc1101_rx_buf_pkt_size == 57 ? app->cc1101_rx_buf_pkt[56] :
                    app->cc1101_rx_buf_pkt_size == 56 ? 55 :
                                                        0;

                if(l && l <= 55 && app->cc1101_rx_buf_pkt[0] == app->cc1101_addr) {
                    /* Send the packet to the main thread */
                    furi_stream_buffer_send(
                        app->rx_stream, app->cc1101_rx_buf_pkt + 1, l, FuriWaitForever);
                }

                break;

            case 3: /* FU3 */
                /* We're only interested in packets that are at least 3 bytes long and
             start with the address, followed by 0x44, followed by the actual
             data */
                if(app->cc1101_rx_buf_pkt_size >= 3 &&
                   app->cc1101_rx_buf_pkt[0] == app->cc1101_addr &&
                   app->cc1101_rx_buf_pkt[1] == 0x44) {
                    /* Send the packet to the main thread */
                    furi_stream_buffer_send(
                        app->rx_stream,
                        app->cc1101_rx_buf_pkt + 2,
                        app->cc1101_rx_buf_pkt_size - 2,
                        FuriWaitForever);
                }

                break;
            }
        }

        /* Should we start sending data, or have we received a GDO0 fall interrupt
       and are we already transmitting? */
        if((evts & send_data && app->receiving) || (evts & gdo0_fall && !app->receiving)) {
            /* Acquire the SPI bus */
            furi_hal_spi_acquire(spi);

            /* Get the current time */
            now_ms = furi_get_tick();

            /* Should we start sending? */
            if(app->receiving) {
                /* Reset the pointer to the first actual data byte to send */
                app->tx_buf_i = 0;

                /* Which method should we use to initiate the transmission? */
                switch(app->hc11_mode) {
                case 1: /* FU1 */

                    /* Prime the counter of wake up packets we have to send first if too
               much time has passed since the last transmission */
                    if(never_sent_anything ||
                       ms_tick_time_diff_ms(now_ms, last_sent_pkt_tstamp) > 100)
                        mode1_wake_up_pkt_ctr = mode1_nb_wake_up_pkts;
                    else
                        mode1_wake_up_pkt_ctr = 0;

                    break;

                case 3: /* FU3 */

                    /* Reset the packet sizes sequence index */
                    mode3_pkt_sizes_seq_i = 0;

                    break;
                }

                /* Bring the CC1101 out of RX and to idle */
                cc1101_set_idle(spi);
            }

            /* No more bytes to send? */
            if(app->tx_buf_i >= app->tx_buf_size) {
                /* Re-enable RX, as the CC1101 is idle as this point since MCSM1 is
           configured to drop the CC1101 to IDLE when it's done transmitting the
           packet */
                cc1101_enable_rx(spi);

                /* Release the SPI bus */
                furi_hal_spi_release(spi);

                app->receiving = true;

                /* Release the sent semaphore */
                furi_semaphore_release(app->cc1101_sent_sem);
            }

            /* We have data to send */
            else {
                /* Actual data bytes left to send */
                pkt_bytes_remaining = app->tx_buf_size - app->tx_buf_i;

                /* Which method should we use to initiate the transmission? */
                switch(app->hc11_mode) {
                case 1: /* FU1 */

                    /* Should we send a wake-up packet? */
                    if(mode1_wake_up_pkt_ctr) {
                        /* Prepare the wake-up packet to send */
                        app->cc1101_tx_buf_pkt[0] = 2;
                        app->cc1101_tx_buf_pkt[1] = app->cc1101_addr;
                        app->cc1101_tx_buf_pkt[2] = 0;
                        app->cc1101_tx_buf_pkt_size = 3;

                        /* Decrement the wake up packet counter */
                        mode1_wake_up_pkt_ctr--;
                    }

                    /* Send actual data */
                    else {
                        /* Prepare the next packet to send */
                        if(pkt_bytes_remaining < 55) {
                            app->cc1101_tx_buf_pkt[0] = 55 + 2;
                            app->cc1101_tx_buf_pkt[1] = app->cc1101_addr;
                            memcpy(
                                app->cc1101_tx_buf_pkt + 2,
                                app->tx_buf + app->tx_buf_i,
                                pkt_bytes_remaining);
                            app->cc1101_tx_buf_pkt[57] = pkt_bytes_remaining;
                            app->tx_buf_i = app->tx_buf_size;
                            app->cc1101_tx_buf_pkt_size = 55 + 3;
                        }

                        else {
                            app->cc1101_tx_buf_pkt[0] = 55 + 1;
                            app->cc1101_tx_buf_pkt[1] = app->cc1101_addr;
                            memcpy(app->cc1101_tx_buf_pkt + 2, app->tx_buf + app->tx_buf_i, 55);
                            app->tx_buf_i += 55;
                            app->cc1101_tx_buf_pkt_size = 55 + 2;
                        }

                        break;

                    case 3: /* FU3 */

                        /* Get the number of bytes we should send from the packet sizes
               sequence */
                        l = mode3_pkt_sizes_seq[mode3_pkt_sizes_seq_i++] - 2;
                        if(mode3_pkt_sizes_seq_i == sizeof(mode3_pkt_sizes_seq))
                            mode3_pkt_sizes_seq_i--;

                        /* Prepare the next packet to send */
                        if(pkt_bytes_remaining < l) {
                            app->cc1101_tx_buf_pkt[0] = pkt_bytes_remaining + 2;
                            app->cc1101_tx_buf_pkt[1] = app->cc1101_addr;
                            app->cc1101_tx_buf_pkt[2] = 0x44;
                            memcpy(
                                app->cc1101_tx_buf_pkt + 3,
                                app->tx_buf + app->tx_buf_i,
                                pkt_bytes_remaining);
                            app->tx_buf_i = app->tx_buf_size;
                            app->cc1101_tx_buf_pkt_size = pkt_bytes_remaining + 3;
                        }

                        else {
                            app->cc1101_tx_buf_pkt[0] = l + 2;
                            app->cc1101_tx_buf_pkt[1] = app->cc1101_addr;
                            app->cc1101_tx_buf_pkt[2] = 0x44;
                            memcpy(app->cc1101_tx_buf_pkt + 3, app->tx_buf + app->tx_buf_i, l);
                            app->tx_buf_i += l;
                            app->cc1101_tx_buf_pkt_size = l + 3;
                        }

                        break;
                    }
                }

                /* Flush the TX FIFO */
                cc1101_strobe(spi, CC1101_STROBE_SFTX);

                /* Write the packet to send to the TX FIFO */
                for(i = 0; i < app->cc1101_tx_buf_pkt_size; i++)
                    cc1101_write_reg(spi, CC1101_FIFO, app->cc1101_tx_buf_pkt[i]);

                /* Log the received packet in the traces log. Only do this if the trace
           log level is enabled to save CPU clock to grab packets faster when
           it normally isn't */
                if(loglevel == FuriLogLevelTrace) {
                    snprintf(
                        app->spstr,
                        sizeof(app->spstr),
                        "Sent %d bytes: ",
                        app->cc1101_tx_buf_pkt_size - 1);
                    l = strlen(app->spstr);
                    for(i = 1; i < app->cc1101_tx_buf_pkt_size; i++)
                        snprintf(
                            app->spstr + l + (i * 3) - 3, 4, "%02X ", app->cc1101_tx_buf_pkt[i]);
                    FURI_LOG_I(TAG, app->spstr);
                }

                /* Enable TX */
                cc1101_enable_tx(spi);

                /* Release the SPI bus */
                furi_hal_spi_release(spi);

                never_sent_anything = false;
                last_sent_pkt_tstamp = now_ms;

                app->receiving = false;
            }
        }
    }

    return 0;
}

/** Main RX thread **/
static int32_t main_rx_thread(void* ctx) {
    HC11RadioCommApp* app = (HC11RadioCommApp*)ctx;
    uint32_t flags;
    uint8_t nb_bytes;

    while(1) {
        /* Get the thread flags (non-blocking) */
        flags = furi_thread_flags_get();

        /* Check for errors */
        furi_check((flags & FuriFlagError) == 0);

        /* Should we stop the thread? */
        if(flags & stop) break;

        /* Try to get data from the RX stream buffer for a while */
        nb_bytes = furi_stream_buffer_receive(app->rx_stream, app->rx_buf, RX_BUF_SIZE, 250);

        /* Did we get something? */
        if(nb_bytes) {
            /* Start a green LED flash */
            start_led_flash(&app->led_control, GREEN);

            /* If we have a raw data handler, call it and pass it the data */
            if(app->raw_data_handler)
                app->raw_data_handler(app->rx_buf, nb_bytes, app->raw_data_handler_ctx);
        }
    }

    return 0;
}

/** Send function **/
void radio_tx(HC11RadioCommApp* app, uint8_t* data, uint16_t len) {
    /* Start a red LED flash */
    start_led_flash(&app->led_control, RED);

    /* save the address of the data and size to send */
    app->tx_buf = data;
    app->tx_buf_size = len;

    /* Tell the HC11 RX/TX thread it has data to send */
    furi_thread_flags_set(furi_thread_get_id(app->hc11_rx_tx_thread), send_data);

    /* Wait for the HC11 RX/TX thread to be done sending the data */
    furi_semaphore_acquire(app->cc1101_sent_sem, FuriWaitForever);
}

/** Initialize the HC11 radio communication app **/
HC11RadioCommApp* hc11_radio_comm_app_init(uint16_t min_led_flash_duration) {
    FURI_LOG_I(TAG, "App init");

    /* Allocate space for the app's structure */
    HC11RadioCommApp* app = malloc(sizeof(HC11RadioCommApp));

    /* No raw data handler callback setup yet */
    app->raw_data_handler = NULL;

    /* Create the semaphore to signal the end of the transmission by the CC1101 */
    app->cc1101_sent_sem = furi_semaphore_alloc(1, 0);

    /* Allocate space for the main RX thread */
    app->main_rx_thread = furi_thread_alloc();

    /* Allocate space for the HC11 RX/TX thread */
    app->hc11_rx_tx_thread = furi_thread_alloc();

    /* Allocate space for the receive stream buffer */
    app->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);

    /* Setup the LED control */
    set_led_control(&app->led_control, min_led_flash_duration);

    /* Initialize the main RX thread */
    furi_thread_set_name(app->main_rx_thread, "hc11_main_rx");
    furi_thread_set_stack_size(app->main_rx_thread, 1024);
    furi_thread_set_context(app->main_rx_thread, app);
    furi_thread_set_callback(app->main_rx_thread, main_rx_thread);

    /* Initialize the HC11 RX/TX thread */
    furi_thread_set_name(app->hc11_rx_tx_thread, "hc11_rx_tx");
    furi_thread_set_stack_size(app->hc11_rx_tx_thread, 2048);
    furi_thread_set_context(app->hc11_rx_tx_thread, app);
    furi_thread_set_callback(app->hc11_rx_tx_thread, hc11_rx_tx_thread);
    furi_thread_set_priority(app->hc11_rx_tx_thread, FuriThreadPriorityIsr);

    /* Start the main RX thread */
    furi_thread_start(app->main_rx_thread);

    /* Start the HC11 RX/TX thread */
    furi_thread_start(app->hc11_rx_tx_thread);

    return app;
}

/** Start the radio **/
void start_radio(
    HC11RadioCommApp* app,
    uint8_t channel,
    uint8_t address,
    uint8_t txpower,
    uint8_t mode) {
    /* Program the channel into the CC1101 presets */
    cc1101_regs[1] = channel << 1;

    /* Program the address into the CC1101 presets */
    cc1101_regs[3] = address;
    app->cc1101_addr = address;

    /* Retrieve the transmission mode */
    app->hc11_mode = mode;

    /* Program the value of PATable[0] corresponding to the TX power into the
     CC1101 presets */
    cc1101_regs[sizeof(cc1101_regs) - 8] = tx_power_to_patable_zero[txpower - 1];

    /* Signal the HC11 RX/TX to start the radio */
    furi_thread_flags_set(furi_thread_get_id(app->hc11_rx_tx_thread), set_cc1101);
}

/** Stop the radio **/
void stop_radio(HC11RadioCommApp* app) {
    /* Signal the HC11 RX/TX to stop the radio */
    furi_thread_flags_set(furi_thread_get_id(app->hc11_rx_tx_thread), unset_cc1101);
}

/** Stop the receive thread and free up the space allocated for the HC11 radio
    communication app **/
void hc11_radio_comm_app_free(HC11RadioCommApp* app) {
    FURI_LOG_I(TAG, "App free");

    /* Stop and free the HC11 RX/TX thread */
    furi_thread_flags_set(furi_thread_get_id(app->hc11_rx_tx_thread), stop);
    furi_thread_join(app->hc11_rx_tx_thread);
    furi_thread_free(app->hc11_rx_tx_thread);

    /* Stop and free the main RX thread */
    furi_thread_flags_set(furi_thread_get_id(app->main_rx_thread), stop);
    furi_thread_join(app->main_rx_thread);
    furi_thread_free(app->main_rx_thread);

    /* Free the receive stream buffer */
    furi_stream_buffer_free(app->rx_stream);

    /* Free the semaphore to signal the end of the transmission by the CC1101 */
    furi_semaphore_free(app->cc1101_sent_sem);

    /* Release the LED control */
    release_led_control(&app->led_control);

    /* Free the HC11 radio communication app's structure */
    free(app);
}
