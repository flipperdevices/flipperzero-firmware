#include "api-hal-subghz.h"
#include <stm32wbxx_ll_gpio.h>
#include <api-hal-gpio.h>
#include <api-hal-spi.h>
#include <cc1101.h>
#include <stdio.h>
#include "main.h"

static const uint8_t api_hal_subghz_preset_tone_rx[][2] = {
    {CC1101_FSCTRL1,    0x06},  // IF frequency
    {CC1101_FSCTRL0,    0x00},  // frequency offset before synthesizer
    {CC1101_MDMCFG4,    0xCC},  // RX filter bandwidth 100k(0xcc)
    {CC1101_MDMCFG3,    0x43},  // datarate config 512kBaud  for the purpose of fast rssi measurement
    {CC1101_MDMCFG1,    0x21},  // FEC preamble etc. last 2 bits for channel spacing
    {CC1101_MDMCFG0,    0xF8},  // 100khz channel spacing
    {CC1101_MCSM0,      0x18},  // calibrate when going from IDLE to RX or TX ; 149 - 155 μs timeout
    {CC1101_FOCCFG,     0x16},  // frequency compensation
    {CC1101_AGCTRL2,    0x43},  
    {CC1101_AGCTRL1,    0x49},  
    {CC1101_AGCTRL0,    0x91},  
    {CC1101_FSCAL3,     0xEA},  
    {CC1101_FSCAL2,     0x2A},  
    {CC1101_FSCAL1,     0x00},  
    {CC1101_FSCAL0,     0x1F},  
    {CC1101_TEST2,      0x81},  
    {CC1101_TEST1,      0x35},  
    {CC1101_TEST0,      0x0B},  // should be 0x0B for lower than 430.6MHz and 0x09 for higher
    {CC1101_IOCFG2,     0x0D},  // data output pin for asynchronous mode
    {CC1101_IOCFG0,     0x2E},  // High impedance (3-state), GDO0 configed as data input for asynchronous mode
    {CC1101_PKTCTRL0,   0x33},  // whitening off; asynchronous serial mode; CRC diable；reserved
    {CC1101_FIFOTHR,    0x47},  // Adc_retention enabled for RX filter bandwidth less than 325KHz; defalut fifo threthold.
    { 0, 0 },
};

static const uint8_t api_hal_subghz_preset_tone_tx[][2] = {
    { CC1101_FIFOTHR,   0x47 }, // 
    { CC1101_PKTCTRL0,  0x32 }, // 
    { CC1101_FSCTRL1,   0x06 }, // 
    { CC1101_FREQ2,     0x10 }, // 
    { CC1101_FREQ1,     0xB0 }, // 
    { CC1101_FREQ0,     0x71 }, // 
    { CC1101_MDMCFG4,   0x6A }, // 
    { CC1101_MDMCFG3,   0x2E }, // 
    { CC1101_MDMCFG2,   0x30 }, // 
    { CC1101_DEVIATN,   0x15 }, // 
    { CC1101_MCSM0,     0x18 }, // 
    { CC1101_FOCCFG,    0x16 }, // 
    { CC1101_WORCTRL,   0xFB }, // 
    { CC1101_FREND0,    0x11 }, // 
    { CC1101_FSCAL3,    0xE9 }, // 
    { CC1101_FSCAL2,    0x2A }, // 
    { CC1101_FSCAL1,    0x00 }, // 
    { CC1101_FSCAL0,    0x1F }, // 
    { 0, 0 },
};

void api_hal_subghz_init() {
    LL_GPIO_SetPinMode(RF_SW_0_GPIO_Port, RF_SW_0_Pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(RF_SW_0_GPIO_Port, RF_SW_0_Pin, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(RF_SW_0_GPIO_Port, RF_SW_0_Pin, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinMode(RF_SW_1_GPIO_Port, RF_SW_1_Pin, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(RF_SW_1_GPIO_Port, RF_SW_1_Pin, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(RF_SW_1_GPIO_Port, RF_SW_1_Pin, LL_GPIO_OUTPUT_PUSHPULL);

    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    // Reset and shutdown
    cc1101_reset(device);
    cc1101_write_reg(device, CC1101_IOCFG0, 0b00001101);
    cc1101_shutdown(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_load_preset(ApiHalSubGhzPreset preset) {
    if(preset == ApiHalSubGhzPresetTestToneRx) {
        api_hal_subghz_load_preset_data(api_hal_subghz_preset_tone_rx);
    } else if(preset == ApiHalSubGhzPresetTestToneTx) {
        api_hal_subghz_load_preset_data(api_hal_subghz_preset_tone_tx);
    }
}

void api_hal_subghz_load_preset_data(const uint8_t data[][2]) {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    uint32_t i = 0;
    while (data[i][0]) {
        cc1101_write_reg(device, data[i][0], data[i][1]);
        i++;
    }
    api_hal_spi_device_return(device);
}

void api_hal_subghz_dump_state() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    printf(
        "[api_hal_subghz] cc1101 chip %d, version %d\r\n",
        cc1101_get_partnumber(device),
        cc1101_get_version(device)
    );
    api_hal_spi_device_return(device);
}

void api_hal_subghz_sleep() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    // Reset and shutdown
    cc1101_shutdown(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_wakeup() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    cc1101_reset(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_idle() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    cc1101_switch_to_idle(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_rx() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    cc1101_switch_to_rx(device);
    api_hal_spi_device_return(device);
}

void api_hal_subghz_tx() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    cc1101_switch_to_tx(device);
    api_hal_spi_device_return(device);
}

float api_hal_subghz_get_rssi() {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);
    int32_t rssi_dec = cc1101_get_rssi(device);
    api_hal_spi_device_return(device);

    float rssi = rssi_dec;
    if(rssi_dec >= 128) {
        rssi = ((rssi - 256.0f) / 2.0f) - 74.0f;
    } else {
        rssi = (rssi / 2.0f) - 74.0f;
    }

    return rssi;
}

uint32_t api_hal_subghz_set_frequency(uint32_t value) {
    const ApiHalSpiDevice* device = api_hal_spi_device_get(ApiHalSpiDeviceIdSubGhz);

    // Compensate rounding
    if (value % cc1101_get_frequency_step(device) > (cc1101_get_frequency_step(device) / 2)) {
        value += cc1101_get_frequency_step(device);
    }

    uint32_t real_frequency = cc1101_set_frequency(device, value);
    cc1101_calibrate(device);

    api_hal_spi_device_return(device);

    return real_frequency;
}

void api_hal_subghz_set_path(ApiHalSubGhzPath path) {
    if (path == ApiHalSubGhzPath1) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPath2) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPath3) {
        LL_GPIO_SetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_SetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else if (path == ApiHalSubGhzPathIsolate) {
        LL_GPIO_ResetOutputPin(RF_SW_0_GPIO_Port, RF_SW_0_Pin);
        LL_GPIO_ResetOutputPin(RF_SW_1_GPIO_Port, RF_SW_1_Pin);
    } else {

    }
}
