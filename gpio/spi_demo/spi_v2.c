/*
Improved demo reading from a w25q32 chip using SPI.
*/

#include <furi.h>
#include <furi_hal.h>

#define TAG "LearnSPI-SPI-V2"

static uint32_t timeout = 1000;
static FuriHalSpiBusHandle* spi = &furi_hal_spi_bus_handle_external;

void read_w25q32_spi_v2() {
    uint8_t command_read_memory[1] = {0x03}; // Read command
    uint8_t read_memory_address[3] = {0x00, 0x01, 0x00}; // Memory address is 0x000100
    uint8_t data_response[6] = {0, 0, 0, 0, 0, 0}; // Read 5 bytes + NULL terminator.

    furi_hal_spi_acquire(spi);

    if(furi_hal_spi_bus_tx(spi, command_read_memory, 1, timeout) &&
       furi_hal_spi_bus_tx(spi, read_memory_address, 3, timeout) &&
       (furi_hal_spi_bus_rx(spi, data_response, 5, timeout))) {
        char* data_response_str = (char*)data_response;
        FURI_LOG_E(
            TAG,
            "MESSAGE STORED IN CHIP AT ADDRESS 0x%02X%02X%02X IS: %s",
            read_memory_address[0],
            read_memory_address[1],
            read_memory_address[2],
            data_response_str);
    } else {
        FURI_LOG_E(TAG, "FAILED - furi_hal_spi_bus_tx or furi_hal_spi_bus_rx failed.");
    }

    furi_hal_spi_release(spi);
}

void spi_demo_v2() {
    furi_hal_spi_bus_handle_init(spi);
    read_w25q32_spi_v2();
    furi_hal_spi_bus_handle_deinit(spi);
}
