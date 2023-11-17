/*
Demo reading from a w25q32 chip using SPI.
*/

#include <furi.h>
#include <furi_hal.h>

#define TAG "LearnSPI-SPI"

static uint32_t timeout = 1000;
static FuriHalSpiBusHandle* spi = &furi_hal_spi_bus_handle_external;

void read_w25q32_spi() {
    uint8_t command_read_memory[10] = {0x03, 0x0, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    uint8_t data_response[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Extra space for NULL termination

    furi_hal_spi_acquire(spi);

    if(furi_hal_spi_bus_trx(spi, command_read_memory, data_response, 9, timeout)) {
        char* data_response_str = (char*)data_response + 4;
        FURI_LOG_E(
            TAG,
            "MESSAGE STORED IN CHIP AT ADDRESS 0x%02X%02X%02X IS: %s",
            command_read_memory[1],
            command_read_memory[2],
            command_read_memory[3],
            data_response_str);
    } else {
        FURI_LOG_E(TAG, "FAILED - furi_hal_spi_bus_trx failed.");
    }

    furi_hal_spi_release(spi);
}

void spi_demo() {
    furi_hal_spi_bus_handle_init(spi);
    read_w25q32_spi();
    furi_hal_spi_bus_handle_deinit(spi);
}
