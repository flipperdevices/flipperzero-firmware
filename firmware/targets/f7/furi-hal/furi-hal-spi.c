#include "furi-hal-spi.h"
#include "furi-hal-resources.h"

#include <stdbool.h>
#include <string.h>
#include <furi.h>

#include <stm32wbxx_ll_spi.h>
#include <stm32wbxx_ll_utils.h>
#include <stm32wbxx_ll_cortex.h>

#define TAG "FuriHalSpi"

static void furi_hal_spi_bus_init(const FuriHalSpiBus* bus) {
    furi_assert(bus);
    furi_assert(bus->mutex);

    if (*bus->mutex == NULL) {
        *bus->mutex = osMutexNew(NULL);
    }

    hal_gpio_init_ex(bus->miso, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, bus->alt_fn);
    hal_gpio_init_ex(bus->mosi, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, bus->alt_fn);
    hal_gpio_init_ex(bus->clk, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedVeryHigh, bus->alt_fn);
}

static void furi_hal_spi_device_cs_init(const FuriHalSpiDevice* device) {
    furi_assert(device);

    hal_gpio_init(
        device->chip_select,
        GpioModeOutputPushPull,
        GpioPullNo,
        GpioSpeedVeryHigh
    );
    hal_gpio_write(device->chip_select, true);
}

void furi_hal_spi_init() {
    for (size_t i=0; i<FuriHalSpiDeviceIdMax; ++i) {
        // for devices that are native to the bus, configure CS immediately
        if (furi_hal_spi_devices[i].main_bus_config == NULL) {
            furi_hal_spi_device_cs_init(&furi_hal_spi_devices[i]);
        }
    }

    furi_hal_spi_bus_init(&spi_r);
    furi_hal_spi_bus_init(&spi_d);

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_spi_bus_lock(const FuriHalSpiBus* bus) {
    furi_assert(bus);
    furi_assert(bus->mutex);
    furi_check(osMutexAcquire(*(bus->mutex), osWaitForever) == osOK);
}

void furi_hal_spi_bus_unlock(const FuriHalSpiBus* bus) {
    furi_assert(bus);
    furi_assert(bus->mutex);
    furi_check(osMutexRelease(*(bus->mutex)) == osOK);
}

void furi_hal_spi_bus_configure(const FuriHalSpiBus* bus, const LL_SPI_InitTypeDef* config) {
    furi_assert(bus);

    LL_SPI_DeInit((SPI_TypeDef*)bus->spi);
    LL_SPI_Init((SPI_TypeDef*)bus->spi, (LL_SPI_InitTypeDef*)config);
    LL_SPI_SetRxFIFOThreshold((SPI_TypeDef*)bus->spi, LL_SPI_RX_FIFO_TH_QUARTER);
    LL_SPI_Enable((SPI_TypeDef*)bus->spi);
}

void furi_hal_spi_bus_end_txrx(const FuriHalSpiBus* bus, uint32_t timeout) {
    while(LL_SPI_GetTxFIFOLevel((SPI_TypeDef *)bus->spi) != LL_SPI_TX_FIFO_EMPTY);
    while(LL_SPI_IsActiveFlag_BSY((SPI_TypeDef *)bus->spi));
    while(LL_SPI_GetRxFIFOLevel((SPI_TypeDef *)bus->spi) != LL_SPI_RX_FIFO_EMPTY) {
        LL_SPI_ReceiveData8((SPI_TypeDef *)bus->spi);
    }
}

bool furi_hal_spi_bus_rx(const FuriHalSpiBus* bus, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(buffer);
    furi_assert(size > 0);

    return furi_hal_spi_bus_trx(bus, buffer, buffer, size, timeout);
}

bool furi_hal_spi_bus_tx(const FuriHalSpiBus* bus, uint8_t* buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(buffer);
    furi_assert(size > 0);
    bool ret = true;

    while(size > 0) {
        if (LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi)) {
            LL_SPI_TransmitData8((SPI_TypeDef *)bus->spi, *buffer);
            buffer++;
            size--;
        }
    }

    furi_hal_spi_bus_end_txrx(bus, timeout);
    LL_SPI_ClearFlag_OVR((SPI_TypeDef *)bus->spi);

    return ret;
}

bool furi_hal_spi_bus_trx(const FuriHalSpiBus* bus, uint8_t* tx_buffer, uint8_t* rx_buffer, size_t size, uint32_t timeout) {
    furi_assert(bus);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);
    furi_assert(size > 0);

    bool ret = true;
    size_t tx_size = size;
    bool tx_allowed = true;

    while(size > 0) {
        if(tx_size > 0 && LL_SPI_IsActiveFlag_TXE((SPI_TypeDef *)bus->spi) && tx_allowed) {
            LL_SPI_TransmitData8((SPI_TypeDef *)bus->spi, *tx_buffer);
            tx_buffer++;
            tx_size--;
            tx_allowed = false;
        }

        if(LL_SPI_IsActiveFlag_RXNE((SPI_TypeDef *)bus->spi)) {
            *rx_buffer = LL_SPI_ReceiveData8((SPI_TypeDef *)bus->spi);
            rx_buffer++;
            size--;
            tx_allowed = true;
        }
    }

    furi_hal_spi_bus_end_txrx(bus, timeout);

    return ret;
}

void furi_hal_spi_device_configure(const FuriHalSpiDevice* device) {
    furi_assert(device);
    furi_assert(device->bus);
    furi_assert(device->config);

    furi_hal_spi_bus_configure(device->bus, device->config);
}


static const FuriHalSpiDevice* furi_hal_spi_device_preconfigure_bus(FuriHalSpiDeviceId device_id) {
    furi_assert(device_id < FuriHalSpiDeviceIdMax);

    const FuriHalSpiDevice* device = &furi_hal_spi_devices[device_id];
    furi_assert(device);
    furi_hal_spi_bus_lock(device->bus);

    // device requires alternate bus configuration - and has delayed CS init
    if (device->main_bus_config != NULL) {
        furi_hal_spi_bus_init(device->bus);
        furi_hal_spi_device_cs_init(device);
    }

    return device;
}

const FuriHalSpiDevice* furi_hal_spi_device_get(FuriHalSpiDeviceId device_id) {
    const FuriHalSpiDevice* device = furi_hal_spi_device_preconfigure_bus(device_id);
    furi_assert(device);
    furi_hal_spi_device_configure(device);
    return device;
}

const FuriHalSpiDevice* furi_hal_spi_custom_device_get(const LL_SPI_InitTypeDef* device_config) {
    if (device_config == NULL) {
        device_config = &furi_hal_spi_config_ext_spi_default;
    }

    const FuriHalSpiDevice* device = furi_hal_spi_device_preconfigure_bus(FuriHalSpiDeviceIdExtSpi);
    furi_assert(device);
    furi_hal_spi_bus_configure(device->bus, device_config);

    return device;
}

void furi_hal_spi_device_return(const FuriHalSpiDevice* device) {
    if (device->main_bus_config != NULL) {
        // restore original bus configuration
        furi_hal_spi_bus_init(device->main_bus_config);
    }
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
