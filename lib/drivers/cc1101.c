#include "cc1101.h"
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

CC1101Status cc1101_write_reg(const ApiHalSpiDevice* device, uint8_t reg, uint8_t data) {
    uint8_t tx[2] = { reg, data };
    CC1101Status rx[2] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, (uint8_t*)rx, 2, osWaitForever);
    hal_gpio_write(device->chip_select, true);

    assert((rx[0].CHIP_RDYn|rx[1].CHIP_RDYn) == 0);
    return rx[1];
}

CC1101Status cc1101_read_reg(const ApiHalSpiDevice* device, uint8_t reg, uint8_t* data) {
    assert(sizeof(CC1101Status) == 1);
    uint8_t tx[2] = { reg|CC1101_READ, 0};
    CC1101Status rx[2] = { 0 };

    hal_gpio_write(device->chip_select, false);
    while(hal_gpio_read(device->bus->miso));
    api_hal_spi_bus_trx(device->bus, tx, (uint8_t*)rx, 2, osWaitForever);
    hal_gpio_write(device->chip_select, true);

    assert((rx[0].CHIP_RDYn) == 0);
    *data = *(uint8_t*)&rx[1];
    return rx[0];
}

uint8_t cc1101_get_partnumber(const ApiHalSpiDevice* device) {
    uint8_t partnumber=0;
    cc1101_read_reg(device, CC1101_STATUS_PARTNUM|CC1101_BURST, &partnumber);
    return partnumber;
}

uint8_t cc1101_get_version(const ApiHalSpiDevice* device) {
    uint8_t version=0;
    cc1101_read_reg(device, CC1101_STATUS_VERSION|CC1101_BURST, &version);
    return version;
}

uint8_t cc1101_get_rssi(const ApiHalSpiDevice* device) {
    uint8_t rssi=0;
    cc1101_read_reg(device, CC1101_STATUS_RSSI|CC1101_BURST, &rssi);
    return rssi;
}

void cc1101_reset(const ApiHalSpiDevice* device) {
    hal_gpio_write(device->chip_select, false);
    delay_us(1000);
    hal_gpio_write(device->chip_select, true);
    delay_us(1000);
    cc1101_strobe(device, CC1101_STROBE_SRES);
}

void cc1101_shutdown(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_SPWD);
}

void cc1101_calibrate(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_SCAL);
}

void cc1101_switch_to_idle(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_SIDLE);
}

void cc1101_switch_to_rx(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_SRX);
}

void cc1101_switch_to_tx(const ApiHalSpiDevice* device) {
    cc1101_strobe(device, CC1101_STROBE_STX);
}

uint32_t cc1101_set_frequency(const ApiHalSpiDevice* device, uint32_t value) {
    uint64_t real_value = (uint64_t)value * 0xFFFF / CC1101_QUARTZ;

    // Sanity check
    assert((real_value & 0xFFFFFF) == real_value);

    cc1101_write_reg(device, CC1101_FREQ2, (real_value >> 16) & 0xFF);
    cc1101_write_reg(device, CC1101_FREQ1, (real_value >> 8 ) & 0xFF);
    cc1101_write_reg(device, CC1101_FREQ0, (real_value >> 0 ) & 0xFF);

    uint64_t real_frequency = real_value * CC1101_QUARTZ / 0xFFFF;

    return (uint32_t)real_frequency;
}

uint32_t cc1101_get_frequency_step(const ApiHalSpiDevice* device) {
    return CC1101_QUARTZ / 0xFFFF;
}

uint32_t cc1101_set_frequency_offset(const ApiHalSpiDevice* device, uint32_t value) {
    uint64_t real_value = value * 0x4000 / CC1101_QUARTZ;
    assert((real_value & 0xFF) == real_value);

    cc1101_write_reg(device, CC1101_FSCTRL0, (real_value >> 0 ) & 0xFF);

    uint64_t real_frequency = real_value * CC1101_QUARTZ / 0x4000;

    return (uint32_t)real_frequency;
}

uint32_t cc1101_get_frequency_offset_step(const ApiHalSpiDevice* device) {
    return CC1101_QUARTZ / 0x4000;
}

void cc1101_set_pa_single(const ApiHalSpiDevice* device, uint8_t value) {

}

void cc1101_set_pa_table(const ApiHalSpiDevice* device, uint8_t value[8]) {

}
