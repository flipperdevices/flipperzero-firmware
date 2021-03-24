#include "cc1101.h"
#include "cc1101_regs.h"
#include <cmsis_os2.h>
#include <api-hal-delay.h>
#include <assert.h>

CC1101Status cc1101_strobe(const ApiHalSpiDevice* device, uint8_t strobe) {
    uint8_t tx[1] = { strobe };
    CC1101Status rx[1] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, (uint8_t*)rx, 1, osWaitForever);
    hal_gpio_write(device->chip_select, true);

    assert(rx[0].CHIP_RDYn == 0);
    return rx[0];
}

uint8_t cc1101_status(const ApiHalSpiDevice* device, uint8_t status) {
    uint8_t tx[1] = { status | CC1101_BURST };
    CC1101Status rx[1] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, (uint8_t*)rx, 1, osWaitForever);
    hal_gpio_write(device->chip_select, true);

    assert(rx[0].CHIP_RDYn == 0);
    return rx[0];
}

CC1101Status cc1101_write_reg(const ApiHalSpiDevice* device, uint8_t reg, uint8_t data) {
    uint8_t tx[2] = { reg | CC1101_WRITE, data };
    CC1101Status rx[2] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, (uint8_t*)rx, 2, osWaitForever);
    hal_gpio_write(device->chip_select, true);

    assert((rx[0].CHIP_RDYn|rx[1].CHIP_RDYn) == 0);
    return rx[1];
}

void cc1101_init(const ApiHalSpiDevice* device) {
    cc1101_reset(device);
    cc1101_sleep(device);
}

void cc1101_reset(const ApiHalSpiDevice* device) {
    hal_gpio_write(device->chip_select, false);
    delay_us(1000);
    hal_gpio_write(device->chip_select, true);
    delay_us(1000);
    cc1101_strobe(device, CC1101_STROBE_SRES);
}

void cc1101_sleep(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_SIDLE);
    cc1101_strobe(device, CC1101_STROBE_SPWD);
}

