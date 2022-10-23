#include <furi_hal.h>
#include <furi_hal_spi_config.h>
#include "spi_mem_tools.h"

static bool spi_mem_tools_trx(
    SPIMemChipCMD cmd,
    uint8_t* tx_buf,
    size_t tx_size,
    uint8_t* rx_buf,
    size_t rx_size) {
    bool success = false;
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    do {
        if(!furi_hal_spi_bus_tx(
               &furi_hal_spi_bus_handle_external, (uint8_t*)&cmd, 1, SPI_MEM_TOOLS_SPI_TIMEOUT))
            break;
        if(tx_buf) {
            if(!furi_hal_spi_bus_tx(
                   &furi_hal_spi_bus_handle_external, tx_buf, tx_size, SPI_MEM_SPI_TIMEOUT))
                break;
        }
        if(rx_buf) {
            if(!furi_hal_spi_bus_rx(
                   &furi_hal_spi_bus_handle_external, rx_buf, rx_size, SPI_MEM_SPI_TIMEOUT))
                break;
        }
        success = true;
    } while(0);
    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
    return success;
}

bool spi_mem_tools_read_chip_info(SPIMemChip* chip) {
    uint8_t rx_buf[3];
    memset(rx_buf, 0, 3);
    if(!spi_mem_tools_trx(SPIMemChipCMDReadJEDECChipID, NULL, 0, rx_buf, 3)) return false;
    if(rx_buf[0] == 0) return false;
    chip->vendor_id = (SPIMemChipVendor)rx_buf[0];
    chip->type_id = rx_buf[1];
    chip->capacity_id = rx_buf[2];
    return true;
}
