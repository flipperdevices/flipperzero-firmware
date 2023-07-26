#include "si4463_ext.h"
#include "config/si4463_config.h"
#include "driver/si446x.h"

#include <furi_hal_region.h>
#include "furi_hal_version.h"
#include "furi_hal_rtc.h"
#include <furi_hal_spi.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>

#include <stm32wbxx_ll_dma.h>
#include <furi_hal_cortex.h>

#include <furi.h>
#include <stdio.h>

#define TAG "SubGhz_Device_Si4463_Ext"

//https://www.silabs.com/documents/public/application-notes/AN633.pdf
//https://d1.amobbs.com/bbs_upload782111/files_52/ourdev_719804UMAFBO.pdf

#define SUBGHZ_DEVICE_SI4463_EXT_TX_GPIO &gpio_ext_pb2
#define SUBGHZ_DEVICE_SI4463_EXT_NSDN_GPIO &gpio_ext_pc3

/* DMA Channels definition */
#define SUBGHZ_DEVICE_SI4463_EXT_DMA DMA2
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_CHANNEL LL_DMA_CHANNEL_3
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_CHANNEL LL_DMA_CHANNEL_4
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_CHANNEL LL_DMA_CHANNEL_5
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_IRQ FuriHalInterruptIdDma2Ch3
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF \
    SUBGHZ_DEVICE_SI4463_EXT_DMA, SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_CHANNEL
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF \
    SUBGHZ_DEVICE_SI4463_EXT_DMA, SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_CHANNEL
#define SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF \
    SUBGHZ_DEVICE_SI4463_EXT_DMA, SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_CHANNEL

/** Low level buffer dimensions and guard times */
#define SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_FULL (256)
#define SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_HALF \
    (SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_FULL / 2)
#define SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_GUARD_TIME 999 << 1

/** SubGhz state */
typedef enum {
    SubGhzDeviceSi4463ExtStateInit, /**< Init pending */
    SubGhzDeviceSi4463ExtStateIdle, /**< Idle, energy save mode */
    SubGhzDeviceSi4463ExtStateAsyncRx, /**< Async RX started */
    SubGhzDeviceSi4463ExtStateAsyncTx, /**< Async TX started, DMA and timer is on */
    SubGhzDeviceSi4463ExtStateAsyncTxEnd, /**< Async TX complete, cleanup needed */
} SubGhzDeviceSi4463ExtState;

/** SubGhz regulation, receive transmission on the current frequency for the
 * region */
typedef enum {
    SubGhzDeviceSi4463ExtRegulationOnlyRx, /**only Rx*/
    SubGhzDeviceSi4463ExtRegulationTxRx, /**TxRx*/
} SubGhzDeviceSi4463ExtRegulation;

typedef struct {
    uint32_t* buffer;
    LevelDuration carry_ld;
    SubGhzDeviceSi4463ExtCallback callback;
    void* callback_context;
    uint32_t gpio_tx_buff[2];
    uint32_t debug_gpio_buff[2];
} SubGhzDeviceSi4463ExtAsyncTx;

typedef struct {
    uint32_t capture_delta_duration;
    SubGhzDeviceSi4463ExtCaptureCallback capture_callback;
    void* capture_callback_context;
} SubGhzDeviceSi4463ExtAsyncRx;

typedef struct {
    volatile SubGhzDeviceSi4463ExtState state;
    volatile SubGhzDeviceSi4463ExtRegulation regulation;
    const GpioPin* async_mirror_pin;
    FuriHalSpiBusHandle* spi_bus_handle;
    const GpioPin* g0_pin;
    SubGhzDeviceSi4463ExtAsyncTx async_tx;
    SubGhzDeviceSi4463ExtAsyncRx async_rx;
} SubGhzDeviceSi4463Ext;

static SubGhzDeviceSi4463Ext* subghz_device_si4463_ext = NULL;

// Apply the radio configuration
void subghz_device_si4463_ext_load_config(const uint8_t config[]) {
    uint8_t buff[17];
    uint8_t buff_tx[2] = {SI446X_CMD_READ_CMD_BUFF, 0xFF};
    uint8_t buff_rx[2] = {0};
    uint16_t i = 0;
    while(config[i]) {
        memcpy(buff, &config[i], sizeof(buff));

        furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
        furi_hal_spi_bus_tx(
            subghz_device_si4463_ext->spi_bus_handle, &buff[1], buff[0], SI446X_TIMEOUT);
        furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);

        buff_rx[1] = 0;
        while(buff_rx[1] != SI446X_CTS_OK) {
            furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
            furi_hal_spi_bus_trx(
                subghz_device_si4463_ext->spi_bus_handle,
                buff_tx,
                (uint8_t*)buff_rx,
                2,
                SI446X_TIMEOUT);
            furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
        }

        i += buff[0];
        i++;
    }
    si446x_clear_interrupt_status(subghz_device_si4463_ext->spi_bus_handle);
}

void subghz_device_si4463_ext_mod_gpio_for_async(SI446X_Prop_Modem_Mod_Type_t modulation) {
    //ASYNC	1	Direct mode operates in asynchronous mode, applies to TX only. GFSK is not supported.
    uint8_t modem_mod[1] = {0};

    switch(modulation) {
    case SI446X_MODEM_MOD_TYPE_MOD_TYPE_CW:
        modem_mod[0] =
            (SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
             SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 |
             SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE | SI446X_MODEM_MOD_TYPE_MOD_TYPE_CW);
        break;
    case SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK:
        modem_mod[0] =
            (SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
             SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 |
             SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE | SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK);
        break;
    case SI446X_MODEM_MOD_TYPE_MOD_TYPE_2FSK:
        modem_mod[0] =
            (SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
             SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 |
             SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE | SI446X_MODEM_MOD_TYPE_MOD_TYPE_2FSK);
        break;
        break;
    case SI446X_MODEM_MOD_TYPE_MOD_TYPE_4FSK:
        modem_mod[0] =
            (SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS |
             SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 |
             SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE | SI446X_MODEM_MOD_TYPE_MOD_TYPE_4FSK);
        break;
    default:
        furi_crash(NULL);
        break;
    }
    si446x_set_properties(
        subghz_device_si4463_ext->spi_bus_handle,
        SI446X_PROP_MODEM_MOD_TYPE,
        &modem_mod[0],
        sizeof(modem_mod));
}

bool subghz_device_si4463_ext_check_init() {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateInit);
    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateIdle;

    subghz_device_si4463_ext_reset();

    subghz_device_si4463_ext_load_config(subghz_device_si4463_preset_ook_650khz_async_regs);
    subghz_device_si4463_ext_dump_state();
    uint8_t buff_tx[] = {SI446X_CMD_FUNC_INFO};
    uint8_t buff_rx[6] = {0};
    si446x_write_data(subghz_device_si4463_ext->spi_bus_handle, &buff_tx[0], sizeof(buff_tx));
    si446x_read_data(subghz_device_si4463_ext->spi_bus_handle, &buff_rx[0], sizeof(buff_rx));

    furi_hal_gpio_init(subghz_device_si4463_ext->g0_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    //ToDo think about where to tie
    si446x_set_pa(subghz_device_si4463_ext->spi_bus_handle, SI446X_SET_MAX_PA);
    subghz_device_si4463_ext_mod_gpio_for_async(SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK);

    //subghz_device_si4463_ext_sleep();

    FURI_LOG_I(TAG, "Init OK");
    return true;
}

bool subghz_device_si4463_ext_alloc() {
    furi_assert(subghz_device_si4463_ext == NULL);
    subghz_device_si4463_ext = malloc(sizeof(SubGhzDeviceSi4463Ext));
    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateInit;
    subghz_device_si4463_ext->regulation = SubGhzDeviceSi4463ExtRegulationTxRx;
    subghz_device_si4463_ext->async_mirror_pin = NULL;
    subghz_device_si4463_ext->spi_bus_handle = &furi_hal_spi_bus_handle_external;
    subghz_device_si4463_ext->g0_pin = SUBGHZ_DEVICE_SI4463_EXT_TX_GPIO;

    subghz_device_si4463_ext->async_rx.capture_delta_duration = 0;

    furi_hal_spi_bus_handle_init(subghz_device_si4463_ext->spi_bus_handle);
    return subghz_device_si4463_ext_check_init();
}

void subghz_device_si4463_ext_free() {
    furi_assert(subghz_device_si4463_ext != NULL);
    furi_hal_spi_bus_handle_deinit(subghz_device_si4463_ext->spi_bus_handle);
    free(subghz_device_si4463_ext);
    subghz_device_si4463_ext = NULL;
}

void subghz_device_si4463_ext_set_async_mirror_pin(const GpioPin* pin) {
    subghz_device_si4463_ext->async_mirror_pin = pin;
}

const GpioPin* subghz_device_si4463_ext_get_data_gpio() {
    return subghz_device_si4463_ext->g0_pin;
}

bool subghz_device_si4463_ext_is_connect() {
    bool ret = false;

    if(subghz_device_si4463_ext == NULL) { // not initialized
        ret = subghz_device_si4463_ext_alloc();
        subghz_device_si4463_ext_sleep();
        subghz_device_si4463_ext_free();
    } else { // initialized
        uint8_t partnumber = si446x_get_partnumber(subghz_device_si4463_ext->spi_bus_handle);
        ret = (partnumber != 0) && (partnumber != 0xFF);
    }

    return ret;
}

void subghz_device_si4463_ext_sleep() {
    subghz_device_si4463_ext_shutdown();
}

void subghz_device_si4463_ext_dump_state() {
    printf(
        "[furi_hal_subghz] si446x chip %X, version %X\r\n",
        si446x_get_partnumber(subghz_device_si4463_ext->spi_bus_handle),
        si446x_get_version(subghz_device_si4463_ext->spi_bus_handle));
    si446x_clear_interrupt_status(subghz_device_si4463_ext->spi_bus_handle);
}

void subghz_device_si4463_set_pa(uint8_t pa) {
    si446x_set_pa(subghz_device_si4463_ext->spi_bus_handle, pa);
}

void subghz_device_si4463_ext_write_packet(const uint8_t* data, uint8_t size) {
    UNUSED(data);
    UNUSED(size);
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_flush_tx(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_write_reg(subghz_device_si4463_ext->spi_bus_handle, Si4463_FIFO, size);
    // Si4463_write_fifo(subghz_device_si4463_ext->spi_bus_handle, data, size);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
}

void subghz_device_si4463_ext_flush_rx() {
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_flush_rx(subghz_device_si4463_ext->spi_bus_handle);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
}

void subghz_device_si4463_ext_flush_tx() {
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_flush_tx(subghz_device_si4463_ext->spi_bus_handle);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
}

bool subghz_device_si4463_ext_rx_pipe_not_empty() {
    // Si4463RxBytes status[1];
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_read_reg(
    //     subghz_device_si4463_ext->spi_bus_handle, (Si4463_STATUS_RXBYTES) | Si4463_BURST, (uint8_t*)status);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
    // // TODO: you can add a buffer overflow flag if needed
    // if(status->NUM_RXBYTES > 0) {
    //     return true;
    // } else {
    //     return false;
    // }

    return true;
}

bool subghz_device_si4463_ext_is_rx_data_crc_valid() {
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // uint8_t data[1];
    // Si4463_read_reg(subghz_device_si4463_ext->spi_bus_handle, Si4463_STATUS_LQI | Si4463_BURST, data);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
    // if(((data[0] >> 7) & 0x01)) {
    //     return true;
    // } else {
    //     return false;
    // }
    return true;
}

void subghz_device_si4463_ext_read_packet(uint8_t* data, uint8_t* size) {
    UNUSED(data);
    UNUSED(size);
    // furi_hal_spi_acquire(subghz_device_si4463_ext->spi_bus_handle);
    // Si4463_read_fifo(subghz_device_si4463_ext->spi_bus_handle, data, size);
    // furi_hal_spi_release(subghz_device_si4463_ext->spi_bus_handle);
}

void subghz_device_si4463_ext_shutdown() {
    furi_hal_gpio_write(SUBGHZ_DEVICE_SI4463_EXT_NSDN_GPIO, true); //nSDN UP
    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateInit;
}

void subghz_device_si4463_ext_reset() {
    furi_hal_gpio_init(
        SUBGHZ_DEVICE_SI4463_EXT_NSDN_GPIO, GpioModeOutputPushPull, GpioPullDown, GpioSpeedLow);
    furi_hal_gpio_init(
        subghz_device_si4463_ext->g0_pin, GpioModeInput, GpioPullDown, GpioSpeedLow); //active go0
    // Reset
    furi_hal_gpio_write(SUBGHZ_DEVICE_SI4463_EXT_NSDN_GPIO, true); //nSDN UP

    FuriHalCortexTimer timer = furi_hal_cortex_timer_get(SI446X_TIMEOUT_NSDN * 1000);
    while(!furi_hal_cortex_timer_is_expired(timer)) {
    } //wait SI446X_TIMEOUT_NSDN

    furi_hal_gpio_write(SUBGHZ_DEVICE_SI4463_EXT_NSDN_GPIO, false); //nSDN DOWN

    //wait CTS
    while(furi_hal_gpio_read(subghz_device_si4463_ext->g0_pin) == false)
        ;
}

void subghz_device_si4463_ext_idle() {
    // //ToDo crutch, GO0 should be low at the time of disengagement
    // furi_hal_gpio_init(
    //     subghz_device_si4463_ext->g0_pin, GpioModeOutputPushPull, GpioPullDown, GpioSpeedLow);
    // furi_hal_gpio_write(subghz_device_si4463_ext->g0_pin, false); //DOWN
    si446x_switch_to_idle(subghz_device_si4463_ext->spi_bus_handle);
    si446x_clear_interrupt_status(subghz_device_si4463_ext->spi_bus_handle);
    //si446x_write_gpio(subghz_device_si4463_ext->spi_bus_handle, SI446X_GPIO1, SI446X_GPIO_MODE_INPUT);
}

void subghz_device_si4463_ext_rx() {
    si446x_write_gpio(subghz_device_si4463_ext->spi_bus_handle, SI446X_GPIO1, SI446X_GPIO_MODE_RX_DATA);
    si446x_write_gpio(
        subghz_device_si4463_ext->spi_bus_handle, SI446X_NIRQ, SI446X_GPIO_MODE_RX_RAW_DATA);
    
    si446x_clear_interrupt_status(subghz_device_si4463_ext->spi_bus_handle);
    uint8_t channel = 0;
    si446x_switch_to_start_rx(
        subghz_device_si4463_ext->spi_bus_handle, channel, SI446X_STATE_NOCHANGE, 0);
}

bool subghz_device_si4463_ext_tx() {
    if(subghz_device_si4463_ext->regulation != SubGhzDeviceSi4463ExtRegulationTxRx) return false;
    si446x_write_gpio(
        subghz_device_si4463_ext->spi_bus_handle, SI446X_GPIO1, SI446X_GPIO_MODE_INPUT);

    si446x_clear_interrupt_status(subghz_device_si4463_ext->spi_bus_handle);
    uint8_t channel = 0;
    return si446x_switch_to_start_tx(
        subghz_device_si4463_ext->spi_bus_handle, channel, SI446X_STATE_NOCHANGE, 0);
}

float subghz_device_si4463_ext_get_rssi() {
    float rssi = (float)si446x_get_rssi(subghz_device_si4463_ext->spi_bus_handle);
    //float rssi = (float)si446x_get_lqi(subghz_device_si4463_ext->spi_bus_handle);
    rssi = (rssi / 2.0f) - 134.0f;
    return rssi;
}

uint8_t subghz_device_si4463_ext_get_lqi() {
    return si446x_get_lqi(subghz_device_si4463_ext->spi_bus_handle);
}

bool subghz_device_si4463_ext_get_properties(SI446X_Prop_t prop, uint8_t* data, uint8_t size) {
    return si446x_get_properties(subghz_device_si4463_ext->spi_bus_handle, prop, data, size);
}

bool subghz_device_si4463_ext_set_properties(SI446X_Prop_t prop, uint8_t* data, uint8_t size) {
    return si446x_set_properties(subghz_device_si4463_ext->spi_bus_handle, prop, data, size);
}

bool subghz_device_si4463_ext_is_frequency_valid(uint32_t value) {
    if(!(value >= 299999755 && value <= 348000335) &&
       !(value >= 386999938 && value <= 464000000) &&
       !(value >= 778999847 && value <= 928000000)) {
        return false;
    }

    return true;
}

uint32_t subghz_device_si4463_ext_set_frequency_and_path(uint32_t value) {
    value = subghz_device_si4463_ext_set_frequency(value);
    if(value >= 299999755 && value <= 348000335) {
        subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath315);
    } else if(value >= 386999938 && value <= 464000000) {
        subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath433);
    } else if(value >= 778999847 && value <= 928000000) {
        subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath868);
    } else {
        furi_crash(NULL);
    }
    return value;
}

uint32_t subghz_device_si4463_ext_set_frequency(uint32_t value) {
    if(furi_hal_region_is_frequency_allowed(value)) {
        subghz_device_si4463_ext->regulation = SubGhzDeviceSi4463ExtRegulationTxRx;
    } else {
        subghz_device_si4463_ext->regulation = SubGhzDeviceSi4463ExtRegulationOnlyRx;
    }
    uint32_t real_frequency = si446x_set_frequency_and_step_channel(
        subghz_device_si4463_ext->spi_bus_handle, value, 250000);
    return real_frequency;
}

void subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath path) {
    //Path_433      sw_0-0 sw_1-1
    //Path_315      sw_0-1 sw_1-0
    //Path_868      sw_0-1 sw_1-1
    //Path_Isolate  sw_0-0 sw_1-0

    //set EXT_PA_RAMP ?????
    // uint8_t pa_mode[1] = {0x88};
    // si446x_set_properties(
    //     subghz_device_si4463_ext->spi_bus_handle,
    //     SI446X_PROP_PA_MODE,
    //     &pa_mode[0],
    //     sizeof(pa_mode));

    if(path == SubGhzDeviceSi4463ExtPath433) {
        si446x_write_sw(
            subghz_device_si4463_ext->spi_bus_handle,
            SI446X_GPIO2,
            SI446X_GPIO_MODE_DRIVE0,
            SI446X_GPIO3,
            SI446X_GPIO_MODE_DRIVE1);
    } else if(path == SubGhzDeviceSi4463ExtPath315) {
        si446x_write_sw(
            subghz_device_si4463_ext->spi_bus_handle,
            SI446X_GPIO2,
            SI446X_GPIO_MODE_DRIVE1,
            SI446X_GPIO3,
            SI446X_GPIO_MODE_DRIVE0);
    } else if(path == SubGhzDeviceSi4463ExtPath868) {
        si446x_write_sw(
            subghz_device_si4463_ext->spi_bus_handle,
            SI446X_GPIO2,
            SI446X_GPIO_MODE_DRIVE1,
            SI446X_GPIO3,
            SI446X_GPIO_MODE_DRIVE1);
    } else if(path == SubGhzDeviceSi4463ExtPathIsolate) {
        si446x_write_sw(
            subghz_device_si4463_ext->spi_bus_handle,
            SI446X_GPIO2,
            SI446X_GPIO_MODE_DRIVE0,
            SI446X_GPIO3,
            SI446X_GPIO_MODE_DRIVE0);
    } else {
        furi_crash(NULL);
    }
}

static bool subghz_device_si4463_ext_start_debug() {
    bool ret = false;
    if(subghz_device_si4463_ext->async_mirror_pin != NULL) {
        furi_hal_gpio_init(
            subghz_device_si4463_ext->async_mirror_pin,
            GpioModeOutputPushPull,
            GpioPullNo,
            GpioSpeedVeryHigh);
        ret = true;
    }
    return ret;
}

static bool subghz_device_si4463_ext_stop_debug() {
    bool ret = false;
    if(subghz_device_si4463_ext->async_mirror_pin != NULL) {
        furi_hal_gpio_init(
            subghz_device_si4463_ext->async_mirror_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        ret = true;
    }
    return ret;
}

static void subghz_device_si4463_ext_capture_ISR() {
    if(!furi_hal_gpio_read(subghz_device_si4463_ext->g0_pin)) {
        if(subghz_device_si4463_ext->async_rx.capture_callback) {
            if(subghz_device_si4463_ext->async_mirror_pin != NULL)
                furi_hal_gpio_write(subghz_device_si4463_ext->async_mirror_pin, false);

            subghz_device_si4463_ext->async_rx.capture_callback(
                true,
                LL_TIM_GetCounter(TIM17) << 1,
                (void*)subghz_device_si4463_ext->async_rx.capture_callback_context);
        }
    } else {
        if(subghz_device_si4463_ext->async_rx.capture_callback) {
            if(subghz_device_si4463_ext->async_mirror_pin != NULL)
                furi_hal_gpio_write(subghz_device_si4463_ext->async_mirror_pin, true);

            subghz_device_si4463_ext->async_rx.capture_callback(
                false,
                LL_TIM_GetCounter(TIM17) << 1,
                (void*)subghz_device_si4463_ext->async_rx.capture_callback_context);
        }
    }
    LL_TIM_SetCounter(TIM17, 4); // 8>>1
}

void subghz_device_si4463_ext_start_async_rx(
    SubGhzDeviceSi4463ExtCaptureCallback callback,
    void* context) {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateIdle);
    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateAsyncRx;

    subghz_device_si4463_ext->async_rx.capture_callback = callback;
    subghz_device_si4463_ext->async_rx.capture_callback_context = context;

    furi_hal_bus_enable(FuriHalBusTIM17);

    // Configure TIM
    LL_TIM_SetPrescaler(TIM17, (64 << 1) - 1);
    LL_TIM_SetCounterMode(TIM17, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM17, 0xFFFF);
    LL_TIM_SetClockDivision(TIM17, LL_TIM_CLOCKDIVISION_DIV1);

    // Timer: advanced
    LL_TIM_SetClockSource(TIM17, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM17);
    LL_TIM_DisableDMAReq_TRIG(TIM17);
    LL_TIM_DisableIT_TRIG(TIM17);

    furi_hal_gpio_init(
        subghz_device_si4463_ext->g0_pin, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_remove_int_callback(subghz_device_si4463_ext->g0_pin);
    furi_hal_gpio_add_int_callback(
        subghz_device_si4463_ext->g0_pin,
        subghz_device_si4463_ext_capture_ISR,
        subghz_device_si4463_ext->async_rx.capture_callback);

    // Start timer
    LL_TIM_SetCounter(TIM17, 0);
    LL_TIM_EnableCounter(TIM17);

    // Start debug
    subghz_device_si4463_ext_start_debug();

    // Switch to RX
    subghz_device_si4463_ext_rx();

    //Clear the variable after the end of the session
    subghz_device_si4463_ext->async_rx.capture_delta_duration = 0;
}

void subghz_device_si4463_ext_stop_async_rx() {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncRx);
    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateIdle;

    // Shutdown radio
    subghz_device_si4463_ext_idle();

    FURI_CRITICAL_ENTER();
    furi_hal_bus_disable(FuriHalBusTIM17);

    // Stop debug
    subghz_device_si4463_ext_stop_debug();

    FURI_CRITICAL_EXIT();
    furi_hal_gpio_remove_int_callback(subghz_device_si4463_ext->g0_pin);
    furi_hal_gpio_init(subghz_device_si4463_ext->g0_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

static void subghz_device_si4463_ext_async_tx_refill(uint32_t* buffer, size_t samples) {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncTx);
    while(samples > 0) {
        bool is_odd = samples % 2;
        LevelDuration ld;
        if(level_duration_is_reset(subghz_device_si4463_ext->async_tx.carry_ld)) {
            ld = subghz_device_si4463_ext->async_tx.callback(
                subghz_device_si4463_ext->async_tx.callback_context);
        } else {
            ld = subghz_device_si4463_ext->async_tx.carry_ld;
            subghz_device_si4463_ext->async_tx.carry_ld = level_duration_reset();
        }

        if(level_duration_is_wait(ld)) {
            *buffer = SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_GUARD_TIME;
            buffer++;
            samples--;
        } else if(level_duration_is_reset(ld)) {
            *buffer = 0;
            buffer++;
            samples--;
            LL_DMA_DisableIT_HT(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
            LL_DMA_DisableIT_TC(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
            LL_TIM_EnableIT_UPDATE(TIM17);
            break;
        } else {
            bool level = level_duration_get_level(ld);

            // Inject guard time if level is incorrect
            if(is_odd != level) {
                *buffer = SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_GUARD_TIME;
                buffer++;
                samples--;

                // Special case: prevent buffer overflow if sample is last
                if(samples == 0) {
                    subghz_device_si4463_ext->async_tx.carry_ld = ld;
                    break;
                }
            }

            uint32_t duration = level_duration_get_duration(ld);
            furi_assert(duration > 0);
            *buffer = duration >> 1;
            buffer++;
            samples--;
        }
    }
}

static void subghz_device_si4463_ext_async_tx_dma_isr() {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncTx);

#if SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_CHANNEL == LL_DMA_CHANNEL_3
    if(LL_DMA_IsActiveFlag_HT3(SUBGHZ_DEVICE_SI4463_EXT_DMA)) {
        LL_DMA_ClearFlag_HT3(SUBGHZ_DEVICE_SI4463_EXT_DMA);
        subghz_device_si4463_ext_async_tx_refill(
            subghz_device_si4463_ext->async_tx.buffer,
            SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_HALF);
    }
    if(LL_DMA_IsActiveFlag_TC3(SUBGHZ_DEVICE_SI4463_EXT_DMA)) {
        LL_DMA_ClearFlag_TC3(SUBGHZ_DEVICE_SI4463_EXT_DMA);
        subghz_device_si4463_ext_async_tx_refill(
            subghz_device_si4463_ext->async_tx.buffer +
                SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_HALF,
            SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_HALF);
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void subghz_device_si4463_ext_async_tx_timer_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM17)) {
        if(LL_TIM_GetAutoReload(TIM17) == 0) {
            LL_DMA_DisableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
            furi_hal_gpio_write(subghz_device_si4463_ext->g0_pin, false);
            if(subghz_device_si4463_ext->async_mirror_pin != NULL)
                furi_hal_gpio_write(subghz_device_si4463_ext->async_mirror_pin, false);
            LL_TIM_DisableCounter(TIM17);
            subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateAsyncTxEnd;
        }
        LL_TIM_ClearFlag_UPDATE(TIM17);
    }
}

bool subghz_device_si4463_ext_start_async_tx(SubGhzDeviceSi4463ExtCallback callback, void* context) {
    furi_assert(subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateIdle);
    furi_assert(callback);

    //If transmission is prohibited by regional settings
    if(subghz_device_si4463_ext->regulation != SubGhzDeviceSi4463ExtRegulationTxRx) return false;

    subghz_device_si4463_ext->async_tx.callback = callback;
    subghz_device_si4463_ext->async_tx.callback_context = context;

    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateAsyncTx;

    subghz_device_si4463_ext->async_tx.buffer =
        malloc(SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_FULL * sizeof(uint32_t));

    //Signal generation with mem-to-mem DMA
    furi_hal_gpio_write(subghz_device_si4463_ext->g0_pin, false);
    furi_hal_gpio_init(
        subghz_device_si4463_ext->g0_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    // Configure DMA  update timer
    LL_DMA_SetMemoryAddress(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF, (uint32_t)subghz_device_si4463_ext->async_tx.buffer);
    LL_DMA_SetPeriphAddress(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF, (uint32_t) & (TIM17->ARR));
    LL_DMA_ConfigTransfer(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_MODE_NORMAL);
    LL_DMA_SetDataLength(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF, SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_FULL);
    LL_DMA_SetPeriphRequest(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF, LL_DMAMUX_REQ_TIM17_UP);

    LL_DMA_EnableIT_TC(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
    LL_DMA_EnableIT_HT(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
    LL_DMA_EnableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);

    furi_hal_interrupt_set_isr(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_IRQ, subghz_device_si4463_ext_async_tx_dma_isr, NULL);

    furi_hal_bus_enable(FuriHalBusTIM17);

    // Configure TIM
    LL_TIM_SetPrescaler(TIM17, (64 << 1) - 1);
    LL_TIM_SetCounterMode(TIM17, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM17, 0xFFFF);
    LL_TIM_SetClockDivision(TIM17, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(TIM17, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM17);

    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTim1TrgComTim17, subghz_device_si4463_ext_async_tx_timer_isr, NULL);

    subghz_device_si4463_ext_async_tx_refill(
        subghz_device_si4463_ext->async_tx.buffer, SUBGHZ_DEVICE_SI4463_EXT_ASYNC_TX_BUFFER_FULL);

    // Configure tx gpio dma
    const GpioPin* gpio = subghz_device_si4463_ext->g0_pin;

    subghz_device_si4463_ext->async_tx.gpio_tx_buff[0] = (uint32_t)gpio->pin << GPIO_NUMBER;
    subghz_device_si4463_ext->async_tx.gpio_tx_buff[1] = gpio->pin;

    LL_DMA_SetMemoryAddress(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF,
        (uint32_t)subghz_device_si4463_ext->async_tx.gpio_tx_buff);
    LL_DMA_SetPeriphAddress(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF, (uint32_t) & (gpio->port->BSRR));
    LL_DMA_ConfigTransfer(
        SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetDataLength(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF, 2);
    LL_DMA_SetPeriphRequest(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF, LL_DMAMUX_REQ_TIM17_UP);
    LL_DMA_EnableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF);

    // Start debug
    if(subghz_device_si4463_ext_start_debug()) {
        gpio = subghz_device_si4463_ext->async_mirror_pin;
        subghz_device_si4463_ext->async_tx.debug_gpio_buff[0] = (uint32_t)gpio->pin << GPIO_NUMBER;
        subghz_device_si4463_ext->async_tx.debug_gpio_buff[1] = gpio->pin;

        LL_DMA_SetMemoryAddress(
            SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF,
            (uint32_t)subghz_device_si4463_ext->async_tx.debug_gpio_buff);
        LL_DMA_SetPeriphAddress(
            SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF, (uint32_t) & (gpio->port->BSRR));
        LL_DMA_ConfigTransfer(
            SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
                LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
                LL_DMA_PRIORITY_LOW);
        LL_DMA_SetDataLength(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF, 2);
        LL_DMA_SetPeriphRequest(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF, LL_DMAMUX_REQ_TIM17_UP);
        LL_DMA_EnableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF);
    }

    // Start counter
    LL_TIM_EnableDMAReq_UPDATE(TIM17);
    LL_TIM_GenerateEvent_UPDATE(TIM17);

    subghz_device_si4463_ext_tx();

    LL_TIM_SetCounter(TIM17, 0);
    LL_TIM_EnableCounter(TIM17);

    return true;
}

bool subghz_device_si4463_ext_is_async_tx_complete() {
    return subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncTxEnd;
}

void subghz_device_si4463_ext_stop_async_tx() {
    furi_assert(
        subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncTx ||
        subghz_device_si4463_ext->state == SubGhzDeviceSi4463ExtStateAsyncTxEnd);

    // Shutdown radio
    subghz_device_si4463_ext_idle();

    // Deinitialize Timer
    FURI_CRITICAL_ENTER();
    furi_hal_bus_disable(FuriHalBusTIM17);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTim1TrgComTim17, NULL, NULL);

    // Deinitialize DMA
    LL_DMA_DeInit(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_DEF);
    LL_DMA_DisableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH4_DEF);
    furi_hal_interrupt_set_isr(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH3_IRQ, NULL, NULL);

    // Deinitialize GPIO
    furi_hal_gpio_write(subghz_device_si4463_ext->g0_pin, false);
    furi_hal_gpio_init(subghz_device_si4463_ext->g0_pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // Stop debug
    if(subghz_device_si4463_ext_stop_debug()) {
        LL_DMA_DisableChannel(SUBGHZ_DEVICE_SI4463_EXT_DMA_CH5_DEF);
    }

    FURI_CRITICAL_EXIT();

    free(subghz_device_si4463_ext->async_tx.buffer);

    subghz_device_si4463_ext->state = SubGhzDeviceSi4463ExtStateIdle;
}
