#include "cc1101.h"
#include "cc1101_regs.h"
#include <cmsis_os2.h>
#include <api-hal-delay.h>

uint8_t cc1101_strobe(const ApiHalSpiDevice* device, uint8_t command) {
    uint8_t tx[1] = { command };
    uint8_t rx[1] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, rx, 1, osWaitForever);
    hal_gpio_write(device->chip_select, true);
    return rx[0];
}

void cc1101_init(const ApiHalSpiDevice* device) {
    cc1101_reset(device);
    delay_us(150000);
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
    cc1101_strobe(device, CC1101_STROBE_SPWD);
}

