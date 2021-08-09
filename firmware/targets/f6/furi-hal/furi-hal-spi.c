#include "furi-hal-spi.h"
#include "furi-hal-resources.h"

#include <stdbool.h>
#include <string.h>
#include <furi.h>

#include <stm32wbxx_ll_spi.h>
#include <stm32wbxx_ll_utils.h>
#include <stm32wbxx_ll_cortex.h>

void furi_hal_spi_init() {
    hal_gpio_init_ex(&gpio_spi_r_miso, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI1);
    hal_gpio_init_ex(&gpio_spi_r_mosi, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI1);
    hal_gpio_init_ex(&gpio_spi_r_sck, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI1);

    hal_gpio_init_ex(&gpio_spi_d_miso, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI2);
    hal_gpio_init_ex(&gpio_spi_d_mosi, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI2);
    hal_gpio_init_ex(&gpio_spi_d_sck, GpioModeAltFunctionPushPull, GpioSpeedVeryHigh, GpioPullNo, GpioAltFn5SPI2);

    // Spi structure is const, but mutex is not
    // Need some hell-ish casting to make it work
    *(osMutexId_t*)spi_r.mutex = osMutexNew(NULL);
    *(osMutexId_t*)spi_d.mutex = osMutexNew(NULL);
    // 
    for (size_t i=0; i<FuriHalSpiDeviceIdMax; ++i) {
        hal_gpio_init(
            furi_hal_spi_devices[i].chip_select,
            GpioModeOutputPushPull,
            GpioPullNo,
            GpioSpeedVeryHigh
        );
    }
    FURI_LOG_I("FuriHalSpi", "Init OK");
}

void furi_hal_spi_bus_lock(const FuriHalSpiBus* bus) {
    furi_assert(bus);
    if (bus->mutex) {
        osMutexAcquire(*bus->mutex, osWaitForever);
    }
}

void furi_hal_spi_bus_unlock(const FuriHalSpiBus* bus) {
    furi_assert(bus);
    if (bus->mutex) {
        osMutexRelease(*bus->mutex);
    }
}

void furi_hal_spi_bus_configure(const FuriHalSpiBus* bus, const LL_SPI_InitTypeDef* config) {
    furi_assert(bus);

    LL_SPI_DeInit((SPI_TypeDef*)bus->spi);
    LL_SPI_Init((SPI_TypeDef*)bus->spi, (LL_SPI_InitTypeDef*)config);
    LL_SPI_SetRxFIFOThreshold((SPI_TypeDef*)bus->spi, LL_SPI_RX_FIFO_TH_QUARTER);
}

void furi_hal_spi_bus_reset(const FuriHalSpiDevice* device) {
    furi_assert(device);

    LL_SPI_DeInit((SPI_TypeDef*)device->bus->spi);
    LL_SPI_Init((SPI_TypeDef*)device->bus->spi, (LL_SPI_InitTypeDef*)device->config);
    LL_SPI_SetRxFIFOThreshold((SPI_TypeDef*)device->bus->spi, LL_SPI_RX_FIFO_TH_QUARTER);
}

bool furi_hal_spi_bus_rx(const FuriHalSpiBus* bus, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(buffer);
    furi_assert(size > 0);
    bool ret = true;

    LL_SPI_Enable((SPI_TypeDef*)bus->spi);
    LL_SPI_SetTransferDirection((SPI_TypeDef *)bus->spi, LL_SPI_FULL_DUPLEX);
    while(size > 0) {
        if (LL_SPI_IsActiveFlag_RXNE((SPI_TypeDef *)bus->spi)) {
            *buffer = LL_SPI_ReceiveData8((SPI_TypeDef *)bus->spi);
            buffer++;
            size--;
        }

        if(LL_SYSTICK_IsActiveCounterFlag()) {
            if(--timeout == 0) {
                ret = false;
                break;
            }
        }
    }
    LL_SPI_Disable((SPI_TypeDef*)bus->spi);

    return ret;
}

bool furi_hal_spi_bus_tx(const FuriHalSpiBus* bus, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(buffer);
    furi_assert(size > 0);
    bool ret = true;

    LL_SPI_Enable((SPI_TypeDef*)bus->spi);
    LL_SPI_SetTransferDirection((SPI_TypeDef *)bus->spi, LL_SPI_FULL_DUPLEX);
    while(size > 0) {
        if (LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi)) {
            LL_SPI_TransmitData8((SPI_TypeDef *)bus->spi, *buffer);
            buffer++;
            size--;
        }

        if(LL_SYSTICK_IsActiveCounterFlag()) {
            if(--timeout == 0) {
                ret = false;
                break;
            }
        }
    }

    while(!LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi));
    while(LL_SPI_GetTxFIFOLevel((SPI_TypeDef *)bus->spi) != LL_SPI_TX_FIFO_EMPTY);

    LL_SPI_Disable((SPI_TypeDef*)bus->spi);

    return ret;
}

bool furi_hal_spi_bus_trx(const FuriHalSpiBus* bus, uint8_t* tx_buffer, uint8_t* rx_buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);
    furi_assert(size > 0);
    bool ret = true;

    size_t tx_size = size;
    LL_SPI_Enable((SPI_TypeDef*)bus->spi);
    LL_SPI_SetTransferDirection((SPI_TypeDef *)bus->spi, LL_SPI_FULL_DUPLEX);
    while(size > 0) {
        if (tx_size > 0 && LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi)) {
            LL_SPI_TransmitData8((SPI_TypeDef *)bus->spi, *tx_buffer);
            tx_buffer++;
            tx_size--;
        }
        if(LL_SPI_IsActiveFlag_RXNE((SPI_TypeDef *)bus->spi)) {
            *rx_buffer = LL_SPI_ReceiveData8((SPI_TypeDef *)bus->spi);
            rx_buffer++;
            size--;
        }

        if(LL_SYSTICK_IsActiveCounterFlag()) {
            if(--timeout == 0) {
                ret = false;
                break;
            }
        }
    }

    while(!LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi));
    while(LL_SPI_GetTxFIFOLevel((SPI_TypeDef *)bus->spi) != LL_SPI_TX_FIFO_EMPTY);

    LL_SPI_Disable((SPI_TypeDef*)bus->spi);

    return ret;
}

const FuriHalSpiDevice* furi_hal_spi_device_get(FuriHalSpiDeviceId device_id) {
    furi_assert(device_id < FuriHalSpiDeviceIdMax);

    const FuriHalSpiDevice* device = &furi_hal_spi_devices[device_id];
    assert(device);
    furi_hal_spi_bus_lock(device->bus);

    if (device->config) {
        furi_hal_spi_bus_configure(device->bus, device->config);
    }

    return device;
}

void furi_hal_spi_device_return(const FuriHalSpiDevice* device) {
    furi_hal_spi_bus_unlock(device->bus);
}

bool furi_hal_spi_device_rx(const FuriHalSpiDevice* device, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(device);
    furi_assert(buffer);
    furi_assert(size > 0);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, false);
    }

    bool ret = furi_hal_spi_bus_rx(device->bus, buffer, size, timeout);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, true);
    }

    return ret;
}

bool furi_hal_spi_device_tx(const FuriHalSpiDevice* device, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(device);
    furi_assert(buffer);
    furi_assert(size > 0);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, false);
    }

    bool ret = furi_hal_spi_bus_tx(device->bus, buffer, size, timeout);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, true);
    }

    return ret;
}

bool furi_hal_spi_device_trx(const FuriHalSpiDevice* device, uint8_t* tx_buffer, uint8_t* rx_buffer, size_t size, uint32_t timeout) {
    furi_assert(device);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);
    furi_assert(size > 0);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, false);
    }

    bool ret = furi_hal_spi_bus_trx(device->bus, tx_buffer, rx_buffer, size, timeout);

    if (device->chip_select) {
        hal_gpio_write(device->chip_select, true);
    }

    return ret;
}
