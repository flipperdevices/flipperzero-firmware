#include "max31855.h"

#include <furi.h>
#include <furi_hal.h>

void max31855_open() {
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_external);
    furi_hal_power_enable_otg();
}

void max31855_close() {
    furi_hal_power_disable_otg();
    furi_hal_spi_bus_handle_deinit(&furi_hal_spi_bus_handle_external);
}

uint32_t max31855_read() {
    uint32_t data = 0;
    uint8_t buffer[4] = {0};

    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    furi_hal_spi_bus_rx(
        &furi_hal_spi_bus_handle_external, buffer, sizeof(buffer), FuriWaitForever);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);

    data = buffer[0];
    data <<= 8;
    data |= buffer[1];
    data <<= 8;
    data |= buffer[2];
    data <<= 8;
    data |= buffer[3];

    return data;
}
