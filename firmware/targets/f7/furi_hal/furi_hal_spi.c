#include "furi_hal_spi.h"
#include "furi_hal_resources.h"
#include <furi_hal_power.h>

#include <stdbool.h>
#include <string.h>
#include <furi.h>

#include <stm32wbxx_ll_spi.h>
#include <stm32wbxx_ll_utils.h>
#include <stm32wbxx_ll_cortex.h>

#define TAG "FuriHalSpi"

void furi_hal_spi_init_early() {
    furi_hal_spi_bus_init(&furi_hal_spi_bus_d);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_display);
}

void furi_hal_spi_deinit_early() {
    furi_hal_spi_bus_handle_deinit(&furi_hal_spi_bus_handle_display);
    furi_hal_spi_bus_deinit(&furi_hal_spi_bus_d);
}

void furi_hal_spi_init() {
    furi_hal_spi_bus_init(&furi_hal_spi_bus_r);

    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_subghz);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_nfc);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_sd_fast);
    furi_hal_spi_bus_handle_init(&furi_hal_spi_bus_handle_sd_slow);

    furi_hal_spi_bus_init(&furi_hal_spi_sw_bus);
    // TODO: this has to be initialized only if external subghz is configured
    furi_hal_spi_bus_handle_init(&furi_hal_spi_sw_bus_handle_subghz_ext);

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_spi_bus_init(FuriHalSpiBus* bus) {
    furi_assert(bus);
    bus->callback(bus, FuriHalSpiBusEventInit);
}

void furi_hal_spi_bus_deinit(FuriHalSpiBus* bus) {
    furi_assert(bus);
    bus->callback(bus, FuriHalSpiBusEventDeinit);
}

void furi_hal_spi_bus_handle_init(FuriHalSpiBusHandle* handle) {
    furi_assert(handle);
    handle->callback(handle, FuriHalSpiBusHandleEventInit);
}

void furi_hal_spi_bus_handle_deinit(FuriHalSpiBusHandle* handle) {
    furi_assert(handle);
    handle->callback(handle, FuriHalSpiBusHandleEventDeinit);
}

void furi_hal_spi_acquire(FuriHalSpiBusHandle* handle) {
    furi_assert(handle);

    furi_hal_power_insomnia_enter();

    handle->bus->callback(handle->bus, FuriHalSpiBusEventLock);
    handle->bus->callback(handle->bus, FuriHalSpiBusEventActivate);

    furi_assert(handle->bus->current_handle == NULL);

    handle->bus->current_handle = handle;
    handle->callback(handle, FuriHalSpiBusHandleEventActivate);
}

void furi_hal_spi_release(FuriHalSpiBusHandle* handle) {
    furi_assert(handle);
    furi_assert(handle->bus->current_handle == handle);

    // Handle event and unset handle
    handle->callback(handle, FuriHalSpiBusHandleEventDeactivate);
    handle->bus->current_handle = NULL;

    // Bus events
    handle->bus->callback(handle->bus, FuriHalSpiBusEventDeactivate);
    handle->bus->callback(handle->bus, FuriHalSpiBusEventUnlock);

    furi_hal_power_insomnia_exit();
}

// Checks that bus is hardware
inline static bool furi_hal_spi_bus_is_hw(FuriHalSpiBus* bus) {
    return bus->spi != NULL;
}


inline static uint8_t furi_hal_spi_sw_bus_txrx(FuriHalSpiBusHandle* handle, uint8_t dat) {
    uint8_t dat_in = 0;

    for (uint8_t mask = 0x80; mask > 0; mask >>= 1) {
        furi_hal_gpio_write(handle->mosi, dat & mask);
        furi_hal_gpio_write(handle->sck, true);
        if (furi_hal_gpio_read(handle->miso))
            dat_in |= mask;
        furi_hal_gpio_write(handle->sck, false);
    }
    return dat_in;
}


static void furi_hal_spi_bus_end_txrx(FuriHalSpiBusHandle* handle, uint32_t timeout) {
    UNUSED(timeout); // FIXME
    while(LL_SPI_GetTxFIFOLevel(handle->bus->spi) != LL_SPI_TX_FIFO_EMPTY)
        ;
    while(LL_SPI_IsActiveFlag_BSY(handle->bus->spi))
        ;
    while(LL_SPI_GetRxFIFOLevel(handle->bus->spi) != LL_SPI_RX_FIFO_EMPTY) {
        LL_SPI_ReceiveData8(handle->bus->spi);
    }
}

bool furi_hal_spi_bus_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* buffer,
    size_t size,
    uint32_t timeout) {
    furi_assert(handle);
    furi_assert(handle->bus->current_handle == handle);
    furi_assert(buffer);
    furi_assert(size > 0);

    return furi_hal_spi_bus_trx(handle, buffer, buffer, size, timeout);
}

bool furi_hal_spi_bus_tx(
    FuriHalSpiBusHandle* handle,
    uint8_t* buffer,
    size_t size,
    uint32_t timeout) {
    furi_assert(handle);
    furi_assert(handle->bus->current_handle == handle);
    furi_assert(buffer);
    furi_assert(size > 0);
    bool ret = true;

    if (furi_hal_spi_bus_is_hw(handle->bus)) {
        while(size > 0) {
            if(LL_SPI_IsActiveFlag_TXE(handle->bus->spi)) {
                LL_SPI_TransmitData8(handle->bus->spi, *buffer);
                buffer++;
                size--;
            }
        }
        furi_hal_spi_bus_end_txrx(handle, timeout);
        LL_SPI_ClearFlag_OVR(handle->bus->spi);
    } else {
        while(size > 0) {
            furi_hal_spi_sw_bus_txrx(handle, *buffer);
            buffer++;
            size--;
        }
    }

    return ret;
}

bool furi_hal_spi_bus_trx(
    FuriHalSpiBusHandle* handle,
    uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout) {
    furi_assert(handle);
    furi_assert(handle->bus->current_handle == handle);
    furi_assert(size > 0);

    bool ret = true;
    size_t tx_size = size;

    if (furi_hal_spi_bus_is_hw(handle->bus)) {
        bool tx_allowed = true;

        while(size > 0) {
            if(tx_size > 0 && LL_SPI_IsActiveFlag_TXE(handle->bus->spi) && tx_allowed) {
                if(tx_buffer) {
                    LL_SPI_TransmitData8(handle->bus->spi, *tx_buffer);
                    tx_buffer++;
                } else {
                    LL_SPI_TransmitData8(handle->bus->spi, 0xFF);
                }
                tx_size--;
                tx_allowed = false;
            }

            if(LL_SPI_IsActiveFlag_RXNE(handle->bus->spi)) {
                if(rx_buffer) {
                    *rx_buffer = LL_SPI_ReceiveData8(handle->bus->spi);
                    rx_buffer++;
                } else {
                    LL_SPI_ReceiveData8(handle->bus->spi);
                }
                size--;
                tx_allowed = true;
            }
        }

        furi_hal_spi_bus_end_txrx(handle, timeout);
    } else {
        uint8_t o_dat, i_dat;
        while(size > 0) {
            if (tx_size > 0) {
                o_dat = *tx_buffer;
                tx_buffer++;
                tx_size--;
            } else {
                o_dat = 0xFF;
            }
            i_dat = furi_hal_spi_sw_bus_txrx(handle, o_dat);
            if (rx_buffer) {
                *rx_buffer = i_dat;
                rx_buffer++;
                size--;
            }
        }
    }

    return ret;
}
