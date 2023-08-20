#include "nrf24.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <assert.h>
#include <string.h>

void nrf24_init() {
    // this is needed if multiple SPI devices are connected to the same bus but with different CS pins
    if(CFW_SETTINGS()->spi_nrf24_handle == SpiDefault) {
        furi_hal_gpio_init_simple(&gpio_ext_pc3, GpioModeOutputPushPull);
        furi_hal_gpio_write(&gpio_ext_pc3, true);
    } else if(CFW_SETTINGS()->spi_nrf24_handle == SpiExtra) {
        furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
        furi_hal_gpio_write(&gpio_ext_pa4, true);
    }

    furi_hal_spi_bus_handle_init(nrf24_HANDLE);
    furi_hal_spi_acquire(nrf24_HANDLE);
    furi_hal_gpio_init(nrf24_CE_PIN, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
}

void nrf24_deinit() {
    furi_hal_spi_release(nrf24_HANDLE);
    furi_hal_spi_bus_handle_deinit(nrf24_HANDLE);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    furi_hal_gpio_init(nrf24_CE_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    // resetting the CS pins to floating
    if(CFW_SETTINGS()->spi_nrf24_handle == SpiDefault) {
        furi_hal_gpio_init_simple(&gpio_ext_pc3, GpioModeAnalog);
    } else if(CFW_SETTINGS()->spi_nrf24_handle == SpiExtra) {
        furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeAnalog);
    }
}

void nrf24_spi_trx(
    FuriHalSpiBusHandle* handle,
    uint8_t* tx,
    uint8_t* rx,
    uint8_t size,
    uint32_t timeout) {
    UNUSED(timeout);
    furi_hal_gpio_write(handle->cs, false);
    furi_hal_spi_bus_trx(handle, tx, rx, size, nrf24_TIMEOUT);
    furi_hal_gpio_write(handle->cs, true);
}

uint8_t nrf24_write_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t data) {
    uint8_t tx[2] = {W_REGISTER | (REGISTER_MASK & reg), data};
    uint8_t rx[2] = {0};
    nrf24_spi_trx(handle, tx, rx, 2, nrf24_TIMEOUT);
    return rx[0];
}

uint8_t nrf24_read_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t* data, uint8_t size) {
    uint8_t tx[size + 1];
    uint8_t rx[size + 1];
    memset(rx, 0, size + 1);
    tx[0] = R_REGISTER | (REGISTER_MASK & reg);
    memset(&tx[1], 0, size);
    nrf24_spi_trx(handle, tx, rx, size + 1, nrf24_TIMEOUT);
    memcpy(data, &rx[1], size);
    return rx[0];
}

uint8_t nrf24_flush_rx(FuriHalSpiBusHandle* handle) {
    uint8_t tx[] = {FLUSH_RX};
    uint8_t rx[] = {0};
    nrf24_spi_trx(handle, tx, rx, 1, nrf24_TIMEOUT);
    return rx[0];
}

uint8_t nrf24_get_rdp(FuriHalSpiBusHandle* handle) {
    uint8_t rdp;
    nrf24_read_reg(handle, REG_RDP, &rdp, 1);
    return rdp;
}

uint8_t nrf24_status(FuriHalSpiBusHandle* handle) {
    uint8_t status;
    uint8_t tx[] = {R_REGISTER | (REGISTER_MASK & REG_STATUS)};
    nrf24_spi_trx(handle, tx, &status, 1, nrf24_TIMEOUT);
    return status;
}

uint8_t nrf24_set_idle(FuriHalSpiBusHandle* handle) {
    uint8_t status = 0;
    uint8_t cfg = 0;
    nrf24_read_reg(handle, REG_CONFIG, &cfg, 1);
    cfg &= 0xfc; // clear bottom two bits to power down the radio
    status = nrf24_write_reg(handle, REG_CONFIG, cfg);
    furi_hal_gpio_write(nrf24_CE_PIN, false);
    return status;
}

uint8_t nrf24_set_rx_mode(FuriHalSpiBusHandle* handle, bool nodelay) {
    uint8_t status = 0;
    uint8_t cfg = 0;
    nrf24_read_reg(handle, REG_CONFIG, &cfg, 1);
    cfg |= 0x03; // PWR_UP, and PRIM_RX
    status = nrf24_write_reg(handle, REG_CONFIG, cfg);
    furi_hal_gpio_write(nrf24_CE_PIN, true);
    if(!nodelay) furi_delay_ms(2000);
    return status;
}

bool nrf24_check_connected(FuriHalSpiBusHandle* handle) {
    uint8_t status = nrf24_status(handle);

    if(status != 0x00) {
        return true;
    } else {
        return false;
    }
}