#include "furi_hal_subghz.h"
#include "furi_hal_version.h"
#include "furi_hal_rtc.h"
#include "furi_hal_delay.h"

#include <furi_hal_gpio.h>
#include <furi_hal_spi.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <furi.h>
#include <si446x.h>
#include <stdio.h>

#define TAG "FuriHalSubGhz"

//https://www.silabs.com/documents/public/application-notes/AN633.pdf

static volatile SubGhzState furi_hal_subghz_state = SubGhzStateInit;
static volatile SubGhzRegulation furi_hal_subghz_regulation = SubGhzRegulationTxRx;
static volatile FuriHalSubGhzPreset furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;

/*
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80
#define RF_GPIO_PIN_CFG 0x13, 0x04, 0x15, 0x21, 0x20, 0x67, 0x4B, 0x00
#define GLOBAL_2_0 0x11, 0x00, 0x04, 0x00, 0x52, 0x00, 0x18, 0x30
#define INT_CTL_2_0 0x11, 0x01, 0x01, 0x00, 0x00
#define FRR_CTL_2_0 0x11, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00
#define PREAMBLE_2_0 0x11, 0x10, 0x05, 0x00, 0x00, 0x14, 0x00, 0x0F, 0x11
#define SYNC_2_0 0x11, 0x11, 0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00
#define PKT_2_0 0x11, 0x12, 0x07, 0x00, 0x04, 0x00, 0x30, 0xFF, 0xFF, 0x20, 0x02
#define PKT_2_1 0x11, 0x12, 0x05, 0x0E, 0x07, 0x04, 0x80, 0x00, 0x80
#define MODEM_2_0 0x11, 0x20, 0x06, 0x00, 0x01, 0x00, 0x07, 0x01, 0x86, 0xA0
#define MODEM_2_1 0x11, 0x20, 0x02, 0x0B, 0x00, 0x00
#define MODEM_2_2 \
    0x11, 0x20, 0x0C, 0x18, 0x00, 0x00, 0x08, 0x03, 0x80, 0x00, 0x0A, 0x30, 0x00, 0xE8, 0x00, 0x5E
#define MODEM_2_3 \
    0x11, 0x20, 0x0C, 0x24, 0x05, 0x76, 0x1A, 0x02, 0xB9, 0x02, 0xC0, 0x00, 0x00, 0x12, 0x00, 0x57
#define MODEM_2_4 0x11, 0x20, 0x06, 0x30, 0x26, 0xFA, 0xA0, 0x00, 0x00, 0x62
#define MODEM_2_5 \
    0x11, 0x20, 0x0B, 0x39, 0x15, 0x15, 0x80, 0x02, 0xFF, 0xFF, 0x00, 0x28, 0x0C, 0xA4, 0x20
#define MODEM_2_6 0x11, 0x20, 0x09, 0x45, 0x03, 0x07, 0xFF, 0x01, 0x00, 0xFF, 0x08, 0x00, 0x18
#define MODEM_2_7 0x11, 0x20, 0x02, 0x50, 0x94, 0x0A
#define MODEM_2_8 0x11, 0x20, 0x02, 0x54, 0x03, 0x07
#define MODEM_2_9 0x11, 0x20, 0x05, 0x5B, 0x40, 0x04, 0x0F, 0x78, 0x20
#define MODEM_CHFLT_2_0 \
    0x11, 0x21, 0x0C, 0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08
#define MODEM_CHFLT_2_1 \
    0x11, 0x21, 0x0C, 0x0C, 0x07, 0x03, 0x15, 0xFC, 0x0F, 0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE
#define MODEM_CHFLT_2_2 \
    0x11, 0x21, 0x0B, 0x18, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x07, 0x03, 0x15, 0xFC, 0x0F
#define FREQ_CONTROL_2_0 0x11, 0x40, 0x08, 0x00, 0x38, 0x0E, 0xD9, 0x16, 0x44, 0x44, 0x20, 0xFE

#define RADIO_CONFIGURATION_DATA_ARRAY                                                           \
    {                                                                                            \
        0x07, RF_POWER_UP, 0x08, RF_GPIO_PIN_CFG, 0x08, GLOBAL_2_0, 0x05, INT_CTL_2_0, 0x07,     \
            FRR_CTL_2_0, 0x09, PREAMBLE_2_0, 0x09, SYNC_2_0, 0x0B, PKT_2_0, 0x09, PKT_2_1, 0x0A, \
            MODEM_2_0, 0x06, MODEM_2_1, 0x10, MODEM_2_2, 0x10, MODEM_2_3, 0x0A, MODEM_2_4, 0x0F, \
            MODEM_2_5, 0x0D, MODEM_2_6, 0x06, MODEM_2_7, 0x06, MODEM_2_8, 0x09, MODEM_2_9, 0x10, \
            MODEM_CHFLT_2_0, 0x10, MODEM_CHFLT_2_1, 0x0F, MODEM_CHFLT_2_2, 0x0C,                 \
            FREQ_CONTROL_2_0,                                                                    \
    }
*/

//tx
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80
#define RF_GPIO_PIN_CFG 0x13, 0x04, 0x04, 0x21, 0x20, 0x67, 0x4B, 0x00
#define GLOBAL_2_0 0x11, 0x00, 0x04, 0x00, 0x52, 0x00, 0x18, 0x30
#define MODEM_2_0 \
    0x11, 0x20, 0x0C, 0x00, 0x0B, 0x00, 0x07, 0x02, 0x71, 0x00, 0x05, 0xC9, 0xC3, 0x80, 0x00, 0x00
#define MODEM_2_1 0x11, 0x20, 0x01, 0x0C, 0x46
#define MODEM_2_2 \
    0x11, 0x20, 0x0C, 0x1C, 0x80, 0x00, 0xB0, 0x10, 0x0C, 0xE8, 0x00, 0x4E, 0x06, 0x8D, 0xB9, 0x00
#define MODEM_2_3 \
    0x11, 0x20, 0x0A, 0x28, 0x00, 0x02, 0xC0, 0x08, 0x00, 0x12, 0x00, 0x23, 0x01, 0x5C
#define MODEM_2_4 \
    0x11, 0x20, 0x0B, 0x39, 0x11, 0x11, 0x80, 0x1A, 0x20, 0x00, 0x00, 0x28, 0x0C, 0xA4, 0x23
#define MODEM_2_5 0x11, 0x20, 0x09, 0x45, 0x03, 0x00, 0x85, 0x01, 0x00, 0xFF, 0x06, 0x09, 0x10
#define MODEM_2_6 0x11, 0x20, 0x02, 0x50, 0x94, 0x0A
#define MODEM_2_7 0x11, 0x20, 0x02, 0x54, 0x03, 0x07
#define MODEM_2_8 0x11, 0x20, 0x05, 0x5B, 0x40, 0x04, 0x04, 0x78, 0x20
#define MODEM_CHFLT_2_0 \
    0x11, 0x21, 0x0C, 0x00, 0x7E, 0x64, 0x1B, 0xBA, 0x58, 0x0B, 0xDD, 0xCE, 0xD6, 0xE6, 0xF6, 0x00
#define MODEM_CHFLT_2_1 \
    0x11, 0x21, 0x0C, 0x0C, 0x03, 0x03, 0x15, 0xF0, 0x3F, 0x00, 0x7E, 0x64, 0x1B, 0xBA, 0x58, 0x0B
#define MODEM_CHFLT_2_2 \
    0x11, 0x21, 0x0B, 0x18, 0xDD, 0xCE, 0xD6, 0xE6, 0xF6, 0x00, 0x03, 0x03, 0x15, 0xF0, 0x3F
#define PA_2_0 0x11, 0x22, 0x01, 0x03, 0x1F
#define FREQ_CONTROL_2_0 0x11, 0x40, 0x08, 0x00, 0x37, 0x09, 0x00, 0x00, 0x44, 0x44, 0x20, 0xFE
#define RF_START_RX 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#define RF_IRCAL 0x17, 0x56, 0x10, 0xCA, 0xF0
#define RF_IRCAL_1 0x17, 0x13, 0x10, 0xCA, 0xF0
#define INT_CTL_5_0 0x11, 0x01, 0x01, 0x00, 0x00
#define FRR_CTL_5_0 0x11, 0x02, 0x04, 0x00, 0x0A, 0x09, 0x04, 0x05
#define PREAMBLE_5_0 0x11, 0x10, 0x05, 0x00, 0x00, 0x14, 0x00, 0x0F, 0x11
#define SYNC_5_0 0x11, 0x11, 0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00
#define PKT_5_0 0x11, 0x12, 0x07, 0x00, 0x04, 0x01, 0x08, 0xFF, 0xFF, 0x20, 0x40
#define PKT_5_1 0x11, 0x12, 0x06, 0x0E, 0x01, 0x06, 0xC2, 0x00, 0x80, 0x02
#define PKT_5_2 0x11, 0x12, 0x05, 0x22, 0x01, 0x06, 0xC2, 0x07, 0xFF
#define MODEM_5_0 0x11, 0x20, 0x07, 0x00, 0x09, 0x00, 0x07, 0x00, 0x5D, 0xC0, 0x01
#define MODEM_5_1 0x11, 0x20, 0x01, 0x0C, 0x00
#define MODEM_5_2 \
    0x11, 0x20, 0x0C, 0x18, 0x00, 0x80, 0x08, 0x03, 0x80, 0x00, 0x0A, 0x30, 0x0C, 0xE8, 0x01, 0x87
#define MODEM_5_3 \
    0x11, 0x20, 0x0C, 0x24, 0x01, 0x4F, 0x8B, 0x00, 0xA8, 0x00, 0xC2, 0x00, 0x54, 0x23, 0x00, 0x15
#define MODEM_5_4 0x11, 0x20, 0x06, 0x30, 0x54, 0x2F, 0x80, 0x00, 0x00, 0x62
#define MODEM_5_5 \
    0x11, 0x20, 0x0B, 0x39, 0x56, 0x56, 0x80, 0x02, 0xFF, 0xFF, 0x00, 0x2A, 0x0C, 0x84, 0x21
#define MODEM_5_6 0x11, 0x20, 0x09, 0x45, 0x8D, 0x00, 0xCC, 0x01, 0x00, 0x80, 0x08, 0x02, 0x18
#define MODEM_5_7 0x11, 0x20, 0x01, 0x5D, 0x0D
#define MODEM_CHFLT_5_0 \
    0x11, 0x21, 0x0C, 0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08
#define MODEM_CHFLT_5_1 \
    0x11, 0x21, 0x0C, 0x0C, 0x07, 0x03, 0x15, 0xFC, 0x0F, 0x00, 0xA2, 0x81, 0x26, 0xAF, 0x3F, 0xEE
#define MODEM_CHFLT_5_2 \
    0x11, 0x21, 0x0B, 0x18, 0xC8, 0xC7, 0xDB, 0xF2, 0x02, 0x08, 0x07, 0x03, 0x15, 0xFC, 0x0F
#define PA_5_0 0x11, 0x22, 0x01, 0x03, 0x5F
#define FREQ_CONTROL_5_0 0x11, 0x40, 0x04, 0x00, 0x38, 0x0E, 0xD9, 0x16

#define RADIO_CONFIGURATION_DATA_ARRAY                                                           \
    {                                                                                            \
        0x07, RF_POWER_UP, 0x08, RF_GPIO_PIN_CFG, 0x08, GLOBAL_2_0, 0x10, MODEM_2_0, 0x05,       \
            MODEM_2_1, 0x10, MODEM_2_2, 0x0E, MODEM_2_3, 0x0F, MODEM_2_4, 0x0D, MODEM_2_5, 0x06, \
            MODEM_2_6, 0x06, MODEM_2_7, 0x09, MODEM_2_8, 0x10, MODEM_CHFLT_2_0, 0x10,            \
            MODEM_CHFLT_2_1, 0x0F, MODEM_CHFLT_2_2, 0x05, PA_2_0, 0x0C, FREQ_CONTROL_2_0, 0x08,  \
            RF_START_RX, 0x05, RF_IRCAL, 0x05, RF_IRCAL_1, 0x05, INT_CTL_5_0, 0x08, FRR_CTL_5_0, \
            0x09, PREAMBLE_5_0, 0x09, SYNC_5_0, 0x0B, PKT_5_0, 0x0A, PKT_5_1, 0x09, PKT_5_2,     \
            0x0B, MODEM_5_0, 0x05, MODEM_5_1, 0x10, MODEM_5_2, 0x10, MODEM_5_3, 0x0A, MODEM_5_4, \
            0x0F, MODEM_5_5, 0x0D, MODEM_5_6, 0x05, MODEM_5_7, 0x10, MODEM_CHFLT_5_0, 0x10,      \
            MODEM_CHFLT_5_1, 0x0F, MODEM_CHFLT_5_2, 0x05, PA_5_0, 0x08, FREQ_CONTROL_5_0,        \
    }

/*
//TX
#define RF_POWER_UP 0x02, 0x01, 0x00, 0x01, 0xC9, 0xC3, 0x80
#define RF_GPIO_PIN_CFG 0x13, 0x15, 0x04, 0x21, 0x20, 0x67, 0x4B, 0x00
#define GLOBAL_2_0 0x11, 0x00, 0x04, 0x00, 0x52, 0x00, 0x18, 0x30
#define INT_CTL_2_0 0x11, 0x01, 0x01, 0x00, 0x00
#define FRR_CTL_2_0 0x11, 0x02, 0x04, 0x00, 0x0A, 0x09, 0x04, 0x05
#define PREAMBLE_2_0 0x11, 0x10, 0x05, 0x00, 0x00, 0x14, 0x00, 0x0F, 0x11
#define SYNC_2_0 0x11, 0x11, 0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00
#define PKT_2_0 0x11, 0x12, 0x06, 0x00, 0x04, 0x01, 0x08, 0xFF, 0xFF, 0x20
#define PKT_2_1 0x11, 0x12, 0x06, 0x0E, 0x01, 0x06, 0xC2, 0x00, 0x80, 0x02
#define PKT_2_2 0x11, 0x12, 0x05, 0x22, 0x01, 0x06, 0xC2, 0x07, 0xFF
#define MODEM_2_0 0x11, 0x20, 0x06, 0x00, 0x09, 0x00, 0x07, 0x00, 0x5D, 0xC0
#define MODEM_2_1 0x11, 0x20, 0x02, 0x0B, 0x00, 0x00
#define MODEM_2_2 \
    0x11, 0x20, 0x0C, 0x18, 0x00, 0x80, 0x08, 0x03, 0x80, 0x00, 0x1A, 0x20, 0x0C, 0xE8, 0x00, 0x62
#define MODEM_2_3 \
    0x11, 0x20, 0x0C, 0x24, 0x05, 0x3E, 0x2D, 0x02, 0x9D, 0x00, 0xC2, 0x00, 0x54, 0x23, 0x00, 0x15
#define MODEM_2_4 0x11, 0x20, 0x06, 0x30, 0x36, 0x23, 0x80, 0x00, 0x00, 0x60
#define MODEM_2_5 \
    0x11, 0x20, 0x0B, 0x39, 0x15, 0x15, 0x80, 0x02, 0xFF, 0xFF, 0x00, 0x28, 0x0C, 0x84, 0x21
#define MODEM_2_6 0x11, 0x20, 0x09, 0x45, 0x8C, 0x01, 0x99, 0x01, 0x00, 0xFF, 0x06, 0x01, 0x18
#define MODEM_2_7 0x11, 0x20, 0x02, 0x50, 0x94, 0x0A
#define MODEM_2_8 0x11, 0x20, 0x02, 0x54, 0x03, 0x07
#define MODEM_2_9 0x11, 0x20, 0x05, 0x5B, 0x40, 0x04, 0x10, 0x78, 0x20
#define MODEM_CHFLT_2_0 \
    0x11, 0x21, 0x0C, 0x01, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03
#define MODEM_CHFLT_2_1 0x11, 0x21, 0x05, 0x0D, 0x00, 0x15, 0xFF, 0x00, 0x00
#define PA_2_0 0x11, 0x22, 0x01, 0x03, 0x5F
#define FREQ_CONTROL_2_0 0x11, 0x40, 0x08, 0x00, 0x38, 0x0E, 0xD9, 0x16, 0x44, 0x44, 0x20, 0xFE

#define RADIO_CONFIGURATION_DATA_ARRAY                                                           \
    {                                                                                            \
        0x07, RF_POWER_UP, 0x08, RF_GPIO_PIN_CFG, 0x08, GLOBAL_2_0, 0x05, INT_CTL_2_0, 0x08,     \
            FRR_CTL_2_0, 0x09, PREAMBLE_2_0, 0x09, SYNC_2_0, 0x0A, PKT_2_0, 0x0A, PKT_2_1, 0x09, \
            PKT_2_2, 0x0A, MODEM_2_0, 0x06, MODEM_2_1, 0x10, MODEM_2_2, 0x10, MODEM_2_3, 0x0A,   \
            MODEM_2_4, 0x0F, MODEM_2_5, 0x0D, MODEM_2_6, 0x06, MODEM_2_7, 0x06, MODEM_2_8, 0x09, \
            MODEM_2_9, 0x10, MODEM_CHFLT_2_0, 0x09, MODEM_CHFLT_2_1, 0x05, PA_2_0, 0x0C,         \
            FREQ_CONTROL_2_0,                                                                    \
    }
*/

static const uint8_t config[] = RADIO_CONFIGURATION_DATA_ARRAY;

// Apply the radio configuration
void furi_hal_subghz_load_config(void) {
    uint8_t buff[17];
    for(uint16_t i = 0; i < sizeof(config); i++) {
        memcpy(buff, &config[i], sizeof(buff));
        si446x_write_data(&furi_hal_spi_bus_handle_subghz, &buff[1], buff[0]);
        i += buff[0];
    }
}

void furi_hal_subghz_init() {
    furi_assert(furi_hal_subghz_state == SubGhzStateInit);
    furi_hal_subghz_state = SubGhzStateIdle;
    furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;

    hal_gpio_init(&gpio_rf_sw_0, GpioModeOutputPushPull, GpioPullDown, GpioSpeedLow);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullDown, GpioSpeedLow); //active go0
    // Reset
    hal_gpio_write(&gpio_rf_sw_0, true); //nSDN UP
    delay_us(10000);
    hal_gpio_write(&gpio_rf_sw_0, false); //nSDN DOWN

    //wait CTS
     while(hal_gpio_read(&gpio_cc1101_g0) == false)
         ;

#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    hal_gpio_init(&FURI_HAL_SUBGHZ_TX_GPIO, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
#endif

    furi_hal_subghz_load_config();
    si446x_clear_interrupt_status(&furi_hal_spi_bus_handle_subghz);
    furi_hal_subghz_dump_state();
    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    //ToDo think about where to tie
    si446x_set_pa(&furi_hal_spi_bus_handle_subghz, 0x5f);
    uint8_t modem_mod[] = {
        (SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
         SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 |
         SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE | SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK)};
    si446x_set_properties(
        &furi_hal_spi_bus_handle_subghz,
        SI446X_PROP_MODEM_MOD_TYPE,
        &modem_mod[0],
        sizeof(modem_mod));

    furi_hal_subghz_sleep();

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_subghz_sleep() {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_INPUT);
    si446x_set_state(&furi_hal_spi_bus_handle_subghz, SI446X_STATE_SLEEP);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;
}

void furi_hal_subghz_dump_state() {
    printf(
        "[furi_hal_subghz] si446x chip %X, version %X\r\n",
        si446x_get_partnumber(&furi_hal_spi_bus_handle_subghz),
        si446x_get_version(&furi_hal_spi_bus_handle_subghz));
}

void furi_hal_subghz_load_preset(FuriHalSubGhzPreset preset) {
    // if(preset == FuriHalSubGhzPresetOok650Async) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_ook_650khz_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable);
    // } else if(preset == FuriHalSubGhzPresetOok270Async) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_ook_270khz_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable);
    // } else if(preset == FuriHalSubGhzPreset2FSKDev238Async) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_2fsk_dev2_38khz_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_2fsk_async_patable);
    // } else if(preset == FuriHalSubGhzPreset2FSKDev476Async) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_2fsk_dev4_76khz_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_2fsk_async_patable);
    // } else if(preset == FuriHalSubGhzPresetMSK99_97KbAsync) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_msk_99_97kb_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_msk_async_patable);
    // } else if(preset == FuriHalSubGhzPresetGFSK9_99KbAsync) {
    //     furi_hal_subghz_load_registers(furi_hal_subghz_preset_gfsk_9_99kb_async_regs);
    //     furi_hal_subghz_load_patable(furi_hal_subghz_preset_gfsk_async_patable);
    // } else {
    //     furi_crash(NULL);
    // }
    furi_hal_subghz_preset = preset;
}

void furi_hal_subghz_load_registers(const uint8_t data[][2]) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_reset(&furi_hal_spi_bus_handle_subghz);
    // uint32_t i = 0;
    // while(data[i][0]) {
    //     cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, data[i][0], data[i][1]);
    //     i++;
    // }
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_load_patable(const uint8_t data[8]) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_set_pa_table(&furi_hal_spi_bus_handle_subghz, data);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_write_packet(const uint8_t* data, uint8_t size) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_flush_tx(&furi_hal_spi_bus_handle_subghz);
    // cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, CC1101_FIFO, size);
    // cc1101_write_fifo(&furi_hal_spi_bus_handle_subghz, data, size);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_flush_rx() {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_flush_rx(&furi_hal_spi_bus_handle_subghz);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_flush_tx() {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_flush_tx(&furi_hal_spi_bus_handle_subghz);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

bool furi_hal_subghz_rx_pipe_not_empty() {
    // CC1101RxBytes status[1];
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_read_reg(
    //     &furi_hal_spi_bus_handle_subghz, (CC1101_STATUS_RXBYTES) | CC1101_BURST, (uint8_t*)status);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    // // TODO: you can add a buffer overflow flag if needed
    // if(status->NUM_RXBYTES > 0) {
    //     return true;
    // } else {
    //     return false;
    // }

    return true;
}

bool furi_hal_subghz_is_rx_data_crc_valid() {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // uint8_t data[1];
    // cc1101_read_reg(&furi_hal_spi_bus_handle_subghz, CC1101_STATUS_LQI | CC1101_BURST, data);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    // if(((data[0] >> 7) & 0x01)) {
    //     return true;
    // } else {
    //     return false;
    // }
    return true;
}

void furi_hal_subghz_read_packet(uint8_t* data, uint8_t* size) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // cc1101_read_fifo(&furi_hal_spi_bus_handle_subghz, data, size);
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_shutdown() {
    hal_gpio_write(&gpio_rf_sw_0, true); //nSDN UP
    furi_hal_subghz_preset = FuriHalSubGhzPresetIDLE;
    furi_hal_subghz_state = SubGhzStateInit;
}

void furi_hal_subghz_reset() {
    // furi_hal_subghz_state = SubGhzStateInit;
    // furi_hal_subghz_init();
    //ToDo reset si446x????
    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    si446x_wait_cts_spi(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_idle() {
    //si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_INPUT);
    //si446x_clear_interrupt_status(&furi_hal_spi_bus_handle_subghz);
    si446x_switch_to_idle(&furi_hal_spi_bus_handle_subghz);
}

void furi_hal_subghz_rx() {
    si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_RX_RAW_DATA);
    si446x_clear_interrupt_status(&furi_hal_spi_bus_handle_subghz);
    uint8_t channel = 0;
    si446x_switch_to_start_rx(&furi_hal_spi_bus_handle_subghz, channel, SI446X_STATE_NOCHANGE, 0);
}

bool furi_hal_subghz_tx() {
    if(furi_hal_subghz_regulation != SubGhzRegulationTxRx) return false;
    si446x_write_gpio(&furi_hal_spi_bus_handle_subghz, SI446X_GPIO1, SI446X_GPIO_MODE_INPUT);
    si446x_clear_interrupt_status(&furi_hal_spi_bus_handle_subghz);
    uint8_t channel = 0;
    return si446x_switch_to_start_tx(
        &furi_hal_spi_bus_handle_subghz, channel, SI446X_STATE_NOCHANGE, 0);
}

float furi_hal_subghz_get_rssi() {
    float rssi = (float)si446x_get_get_rssi(&furi_hal_spi_bus_handle_subghz);
    rssi = (rssi / 2.0f) - 134.0f;
    return rssi;
}

uint8_t furi_hal_subghz_get_lqi() {
    return si446x_get_get_lqi(&furi_hal_spi_bus_handle_subghz);
}

bool furi_hal_subghz_is_frequency_valid(uint32_t value) {
    if(!(value >= 299999755 && value <= 348000335) &&
       !(value >= 386999938 && value <= 464000000) &&
       !(value >= 778999847 && value <= 928000000)) {
        return false;
    }

    return true;
}

uint32_t furi_hal_subghz_set_frequency_and_path(uint32_t value) {
    value = furi_hal_subghz_set_frequency(value);
    if(value >= 299999755 && value <= 348000335) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath315);
    } else if(value >= 386999938 && value <= 464000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath433);
    } else if(value >= 778999847 && value <= 928000000) {
        furi_hal_subghz_set_path(FuriHalSubGhzPath868);
    } else {
        furi_crash(NULL);
    }
    return value;
}

bool furi_hal_subghz_is_tx_allowed(uint32_t value) {
    //checking regional settings
    bool is_allowed = false;
    switch(furi_hal_version_get_hw_region()) {
    case FuriHalVersionRegionEuRu:
        //433,05..434,79; 868,15..868,55
        if(!(value >= 433050000 && value <= 434790000) &&
           !(value >= 868150000 && value <= 868550000)) {
        } else {
            is_allowed = true;
        }
        break;
    case FuriHalVersionRegionUsCaAu:
        //304,10..321,95; 433,05..434,79; 915,00..928,00
        if(!(value >= 304100000 && value <= 321950000) &&
           !(value >= 433050000 && value <= 434790000) &&
           !(value >= 915000000 && value <= 928000000)) {
        } else {
            if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
                if((value >= 304100000 && value <= 321950000) &&
                   ((furi_hal_subghz_preset == FuriHalSubGhzPresetOok270Async) ||
                    (furi_hal_subghz_preset == FuriHalSubGhzPresetOok650Async))) {
                    //furi_hal_subghz_load_patable(furi_hal_subghz_preset_ook_async_patable_au);
                }
            }
            is_allowed = true;
        }
        break;
    case FuriHalVersionRegionJp:
        //312,00..315,25; 920,50..923,50
        if(!(value >= 312000000 && value <= 315250000) &&
           !(value >= 920500000 && value <= 923500000)) {
        } else {
            is_allowed = true;
        }
        break;

    default:
        is_allowed = true;
        break;
    }
    return is_allowed;
}

uint32_t furi_hal_subghz_set_frequency(uint32_t value) {
    if(furi_hal_subghz_is_tx_allowed(value)) {
        furi_hal_subghz_regulation = SubGhzRegulationTxRx;
    } else {
        furi_hal_subghz_regulation = SubGhzRegulationOnlyRx;
    }
    uint32_t real_frequency =
        si446x_set_frequency_and_step_channel(&furi_hal_spi_bus_handle_subghz, value, 250000);
    //ToDo calibration????
    return real_frequency;
    //return 433920000;
}

void furi_hal_subghz_set_path(FuriHalSubGhzPath path) {
    // furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    // if(path == FuriHalSubGhzPath433) {
    //     hal_gpio_write(&gpio_rf_sw_0, 0);
    //     cc1101_write_reg(
    //         &furi_hal_spi_bus_handle_subghz, CC1101_IOCFG2, CC1101IocfgHW | CC1101_IOCFG_INV);
    // } else if(path == FuriHalSubGhzPath315) {
    //     hal_gpio_write(&gpio_rf_sw_0, 1);
    //     cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, CC1101_IOCFG2, CC1101IocfgHW);
    // } else if(path == FuriHalSubGhzPath868) {
    //     hal_gpio_write(&gpio_rf_sw_0, 1);
    //     cc1101_write_reg(
    //         &furi_hal_spi_bus_handle_subghz, CC1101_IOCFG2, CC1101IocfgHW | CC1101_IOCFG_INV);
    // } else if(path == FuriHalSubGhzPathIsolate) {
    //     hal_gpio_write(&gpio_rf_sw_0, 0);
    //     cc1101_write_reg(&furi_hal_spi_bus_handle_subghz, CC1101_IOCFG2, CC1101IocfgHW);
    // } else {
    //     furi_crash(NULL);
    // }
    // furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
}

volatile uint32_t furi_hal_subghz_capture_delta_duration = 0;
volatile FuriHalSubGhzCaptureCallback furi_hal_subghz_capture_callback = NULL;
volatile void* furi_hal_subghz_capture_callback_context = NULL;

static void furi_hal_subghz_capture_ISR() {
    // Channel 1
    if(LL_TIM_IsActiveFlag_CC1(TIM2)) {
        LL_TIM_ClearFlag_CC1(TIM2);
        furi_hal_subghz_capture_delta_duration = LL_TIM_IC_GetCaptureCH1(TIM2);
        if(furi_hal_subghz_capture_callback) {
            furi_hal_subghz_capture_callback(
                true,
                furi_hal_subghz_capture_delta_duration,
                (void*)furi_hal_subghz_capture_callback_context);
        }
    }
    // Channel 2
    if(LL_TIM_IsActiveFlag_CC2(TIM2)) {
        LL_TIM_ClearFlag_CC2(TIM2);
        if(furi_hal_subghz_capture_callback) {
            furi_hal_subghz_capture_callback(
                false,
                LL_TIM_IC_GetCaptureCH2(TIM2) - furi_hal_subghz_capture_delta_duration,
                (void*)furi_hal_subghz_capture_callback_context);
        }
    }
}

void furi_hal_subghz_start_async_rx(FuriHalSubGhzCaptureCallback callback, void* context) {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    furi_hal_subghz_state = SubGhzStateAsyncRx;

    furi_hal_subghz_capture_callback = callback;
    furi_hal_subghz_capture_callback_context = context;

    hal_gpio_init_ex(
        &gpio_cc1101_g0, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 0x7FFFFFFE;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV4;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    // Timer: advanced
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_TI2FP2);
    LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_RESET);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_EnableMasterSlaveMode(TIM2);
    LL_TIM_DisableDMAReq_TRIG(TIM2);
    LL_TIM_DisableIT_TRIG(TIM2);

    // Timer: channel 1 indirect
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);

    // Timer: channel 2 direct
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);

    // ISR setup
    furi_hal_interrupt_set_timer_isr(TIM2, furi_hal_subghz_capture_ISR);
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    // Interrupts and channels
    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_EnableIT_CC2(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    // Enable NVIC
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    // Start timer
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    // Switch to RX
    furi_hal_subghz_rx();
}

void furi_hal_subghz_stop_async_rx() {
    furi_assert(furi_hal_subghz_state == SubGhzStateAsyncRx);
    furi_hal_subghz_state = SubGhzStateIdle;

    // Shutdown radio
    furi_hal_subghz_idle();

    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TIM2);
    FURI_CRITICAL_EXIT();
    furi_hal_interrupt_set_timer_isr(TIM2, NULL);

    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

#define API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL (256)
#define API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF (API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL / 2)
#define API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME 333

typedef struct {
    uint32_t* buffer;
    bool flip_flop;
    FuriHalSubGhzAsyncTxCallback callback;
    void* callback_context;
    uint64_t duty_high;
    uint64_t duty_low;
} FuriHalSubGhzAsyncTx;

static FuriHalSubGhzAsyncTx furi_hal_subghz_async_tx = {0};

static void furi_hal_subghz_async_tx_refill(uint32_t* buffer, size_t samples) {
    while(samples > 0) {
        bool is_odd = samples % 2;
        LevelDuration ld =
            furi_hal_subghz_async_tx.callback(furi_hal_subghz_async_tx.callback_context);

        if(level_duration_is_wait(ld)) {
            return;
        } else if(level_duration_is_reset(ld)) {
            // One more even sample required to end at low level
            if(is_odd) {
                *buffer = API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                buffer++;
                samples--;
                furi_hal_subghz_async_tx.duty_low += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
            }
            break;
        } else {
            // Inject guard time if level is incorrect
            bool level = level_duration_get_level(ld);
            if(is_odd == level) {
                *buffer = API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                buffer++;
                samples--;
                if(!level) {
                    furi_hal_subghz_async_tx.duty_high += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                } else {
                    furi_hal_subghz_async_tx.duty_low += API_HAL_SUBGHZ_ASYNC_TX_GUARD_TIME;
                }
            }

            uint32_t duration = level_duration_get_duration(ld);
            furi_assert(duration > 0);
            *buffer = duration;
            buffer++;
            samples--;

            if(level) {
                furi_hal_subghz_async_tx.duty_high += duration;
            } else {
                furi_hal_subghz_async_tx.duty_low += duration;
            }
        }
    }

    memset(buffer, 0, samples * sizeof(uint32_t));
}

static void furi_hal_subghz_async_tx_dma_isr() {
    furi_assert(furi_hal_subghz_state == SubGhzStateAsyncTx);
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        furi_hal_subghz_async_tx_refill(
            furi_hal_subghz_async_tx.buffer, API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF);
    }
    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        furi_hal_subghz_async_tx_refill(
            furi_hal_subghz_async_tx.buffer + API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF,
            API_HAL_SUBGHZ_ASYNC_TX_BUFFER_HALF);
    }
}

static void furi_hal_subghz_async_tx_timer_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        if(LL_TIM_GetAutoReload(TIM2) == 0) {
            if(furi_hal_subghz_state == SubGhzStateAsyncTx) {
                furi_hal_subghz_state = SubGhzStateAsyncTxLast;
                //forcibly pulls the pin to the ground so that there is no carrier
                hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullDown, GpioSpeedLow);
            } else {
                furi_hal_subghz_state = SubGhzStateAsyncTxEnd;
                LL_TIM_DisableCounter(TIM2);
            }
        }
    }
}

bool furi_hal_subghz_start_async_tx(FuriHalSubGhzAsyncTxCallback callback, void* context) {
    furi_assert(furi_hal_subghz_state == SubGhzStateIdle);
    furi_assert(callback);

    //If transmission is prohibited by regional settings
    if(furi_hal_subghz_regulation != SubGhzRegulationTxRx) return false;

    furi_hal_subghz_async_tx.callback = callback;
    furi_hal_subghz_async_tx.callback_context = context;

    furi_hal_subghz_state = SubGhzStateAsyncTx;

    furi_hal_subghz_async_tx.duty_low = 0;
    furi_hal_subghz_async_tx.duty_high = 0;

    furi_hal_subghz_async_tx.buffer =
        malloc(API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL * sizeof(uint32_t));
    furi_hal_subghz_async_tx_refill(
        furi_hal_subghz_async_tx.buffer, API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL);

    // Connect CC1101_GD0 to TIM2 as output
    hal_gpio_init_ex(
        &gpio_cc1101_g0, GpioModeAltFunctionPushPull, GpioPullDown, GpioSpeedLow, GpioAltFn1TIM2);

    // Configure DMA
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (TIM2->ARR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)furi_hal_subghz_async_tx.buffer;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = API_HAL_SUBGHZ_ASYNC_TX_BUFFER_FULL;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_config);
    furi_hal_interrupt_set_dma_channel_isr(
        DMA1, LL_DMA_CHANNEL_1, furi_hal_subghz_async_tx_dma_isr);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    // Configure TIM2
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 1000;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_EnableARRPreload(TIM2);

    // Configure TIM2 CH2
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_TOGGLE;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_DisableMasterSlaveMode(TIM2);

    furi_hal_interrupt_set_timer_isr(TIM2, furi_hal_subghz_async_tx_timer_isr);
    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    // Start counter
    LL_TIM_GenerateEvent_UPDATE(TIM2);
#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    hal_gpio_write(&FURI_HAL_SUBGHZ_TX_GPIO, true);
#endif
    furi_hal_subghz_tx();

    // Enable NVIC
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    return true;
}

bool furi_hal_subghz_is_async_tx_complete() {
    return furi_hal_subghz_state == SubGhzStateAsyncTxEnd;
}

void furi_hal_subghz_stop_async_tx() {
    furi_assert(
        furi_hal_subghz_state == SubGhzStateAsyncTx ||
        furi_hal_subghz_state == SubGhzStateAsyncTxLast ||
        furi_hal_subghz_state == SubGhzStateAsyncTxEnd);

    // Shutdown radio
    furi_hal_subghz_idle();
#ifdef FURI_HAL_SUBGHZ_TX_GPIO
    hal_gpio_write(&FURI_HAL_SUBGHZ_TX_GPIO, false);
#endif

    // Deinitialize Timer
    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TIM2);
    furi_hal_interrupt_set_timer_isr(TIM2, NULL);

    // Deinitialize DMA
    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_1);
    furi_hal_interrupt_set_dma_channel_isr(DMA1, LL_DMA_CHANNEL_1, NULL);

    // Deinitialize GPIO
    hal_gpio_init(&gpio_cc1101_g0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    FURI_CRITICAL_EXIT();

    free(furi_hal_subghz_async_tx.buffer);

    float duty_cycle =
        100.0f * (float)furi_hal_subghz_async_tx.duty_high /
        ((float)furi_hal_subghz_async_tx.duty_low + (float)furi_hal_subghz_async_tx.duty_high);
    FURI_LOG_D(
        TAG,
        "Async TX Radio stats: on %0.0fus, off %0.0fus, DutyCycle: %0.0f%%",
        (float)furi_hal_subghz_async_tx.duty_high,
        (float)furi_hal_subghz_async_tx.duty_low,
        duty_cycle);

    furi_hal_subghz_state = SubGhzStateIdle;
}
