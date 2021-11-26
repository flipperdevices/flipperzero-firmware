#include <furi-hal-i2c.h>
#include <furi-hal-version.h>

#include <stm32wbxx_ll_i2c.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_cortex.h>

#include <assert.h>

static void furi_hal_i2c_init_bus(FuriHalI2cBus* bus) {
    bus->current_handle = NULL;
}

void furi_hal_i2c_init() {
    furi_hal_i2c_init_bus(&furi_hal_i2c_bus_power);
    furi_hal_i2c_init_bus(&furi_hal_i2c_bus_external);
}

void furi_hal_i2c_acquire(FuriHalI2cBusHandle* handle) {
    assert(handle->bus->current_handle == NULL);

    handle->bus->current_handle = handle;

    handle->bus->callback(handle->bus, FuriHalI2cBusEventInit);

    handle->callback(handle, FuriHalI2cBusHandleEventAttach);
}

void furi_hal_i2c_release(FuriHalI2cBusHandle* handle) {
    assert(handle->bus->current_handle == handle);

    handle->callback(handle, FuriHalI2cBusHandleEventDetach);

    handle->bus->callback(handle->bus, FuriHalI2cBusEventDeinit);

    handle->bus->current_handle = NULL;
}

bool furi_hal_i2c_tx(
    FuriHalI2cBusHandle* handle,
    uint8_t address,
    const uint8_t* data,
    uint8_t size,
    uint32_t timeout) {
    assert(handle->bus->current_handle == handle);
    uint32_t time_left = timeout;
    bool ret = true;

    while(LL_I2C_IsActiveFlag_BUSY(handle->bus->i2c))
        ;

    LL_I2C_HandleTransfer(
        handle->bus->i2c,
        address,
        LL_I2C_ADDRSLAVE_7BIT,
        size,
        LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_WRITE);

    while(!LL_I2C_IsActiveFlag_STOP(handle->bus->i2c) || size > 0) {
        if(LL_I2C_IsActiveFlag_TXIS(handle->bus->i2c)) {
            LL_I2C_TransmitData8(handle->bus->i2c, (*data));
            data++;
            size--;
            time_left = timeout;
        }

        if(LL_SYSTICK_IsActiveCounterFlag()) {
            if(--time_left == 0) {
                ret = false;
                break;
            }
        }
    }

    LL_I2C_ClearFlag_STOP(handle->bus->i2c);

    return ret;
}

bool furi_hal_i2c_rx(
    FuriHalI2cBusHandle* handle,
    uint8_t address,
    uint8_t* data,
    uint8_t size,
    uint32_t timeout) {
    assert(handle->bus->current_handle == handle);
    uint32_t time_left = timeout;
    bool ret = true;

    while(LL_I2C_IsActiveFlag_BUSY(handle->bus->i2c))
        ;

    LL_I2C_HandleTransfer(
        handle->bus->i2c,
        address,
        LL_I2C_ADDRSLAVE_7BIT,
        size,
        LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_READ);

    while(!LL_I2C_IsActiveFlag_STOP(handle->bus->i2c) || size > 0) {
        if(LL_I2C_IsActiveFlag_RXNE(handle->bus->i2c)) {
            *data = LL_I2C_ReceiveData8(handle->bus->i2c);
            data++;
            size--;
            time_left = timeout;
        }

        if(LL_SYSTICK_IsActiveCounterFlag()) {
            if(--time_left == 0) {
                ret = false;
                break;
            }
        }
    }

    LL_I2C_ClearFlag_STOP(handle->bus->i2c);

    return ret;
}

bool furi_hal_i2c_trx(
    FuriHalI2cBusHandle* handle,
    uint8_t address,
    const uint8_t* tx_data,
    uint8_t tx_size,
    uint8_t* rx_data,
    uint8_t rx_size,
    uint32_t timeout) {
    if(furi_hal_i2c_tx(handle, address, tx_data, tx_size, timeout) &&
       furi_hal_i2c_rx(handle, address, rx_data, rx_size, timeout)) {
        return true;
    } else {
        return false;
    }
}
