/**
 * @file furi_hal_i2c.h
 * I2C HAL API
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <furi_hal_cortex.h>
#include <furi_hal_i2c_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Transaction beginning signal */
typedef enum {
    /*! Begin the transaction by sending a START condition followed by the address */
    FuriHalI2cBeginStart,
    /*! Begin the transaction by sending a RESTART condition followed by the address
     * @note Must follow a transaction ended with FuriHalI2cEndAwaitRestart */
    FuriHalI2cBeginRestart,
    /*! Continue the previous transaction with new data
     * @note Must follow a transaction ended with FuriHalI2cEndPause and be of
     * the same type (RX/TX) */
    FuriHalI2cBeginResume,
} FuriHalI2cBegin;

/** Transaction end signal */
typedef enum {
    /*! End the transaction by sending a STOP condition */
    FuriHalI2cEndStop,
    /*! End the transaction by clock stretching
     *  @note Must be followed by a transaction using FuriHalI2cBeginRestart */
    FuriHalI2cEndAwaitRestart,
    /*! Pauses the transaction by clock stretching
     *  @note Must be followed by a transaction using FuriHalI2cBeginResume */
    FuriHalI2cEndPause,
} FuriHalI2cEnd;

/** Early Init I2C */
void furi_hal_i2c_init_early();

/** Early DeInit I2C */
void furi_hal_i2c_deinit_early();

/** Init I2C */
void furi_hal_i2c_init();

/** Acquire i2c bus handle
 *
 * @return     Instance of FuriHalI2cBus
 */
void furi_hal_i2c_acquire(FuriHalI2cBusHandle* handle);

/** Release i2c bus handle
 *
 * @param      bus   instance of FuriHalI2cBus aquired in `furi_hal_i2c_acquire`
 */
void furi_hal_i2c_release(FuriHalI2cBusHandle* handle);

/** Perform I2C tx transfer
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      address  I2C slave address
 * @param      data     pointer to data buffer
 * @param      size     size of data buffer
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_tx(
    FuriHalI2cBusHandle* handle,
    const uint8_t address,
    const uint8_t* data,
    size_t size,
    uint32_t timeout);

/** Perform I2C tx transfer, with additional settings.
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      address  I2C slave address
 * @param      ten_bit  whether the address is 10 bits wide
 * @param      data     pointer to data buffer
 * @param      size     size of data buffer
 * @param      begin    how to begin the transaction
 * @param      end      how to end the transaction
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_tx_ext(
    FuriHalI2cBusHandle* handle,
    uint16_t address,
    bool ten_bit,
    uint8_t const* data,
    size_t size,
    FuriHalI2cBegin begin,
    FuriHalI2cEnd end,
    FuriHalCortexTimer timer);

/** Perform I2C rx transfer
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      address  I2C slave address
 * @param      data     pointer to data buffer
 * @param      size     size of data buffer
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_rx(
    FuriHalI2cBusHandle* handle,
    const uint8_t address,
    uint8_t* data,
    size_t size,
    uint32_t timeout);

/** Perform I2C xx transfer, with additional settings.
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      address  I2C slave address
 * @param      ten_bit  whether the address is 10 bits wide
 * @param      data     pointer to data buffer
 * @param      size     size of data buffer
 * @param      begin    how to begin the transaction
 * @param      end      how to end the transaction
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_rx_ext(
    FuriHalI2cBusHandle* handle,
    uint16_t address,
    bool ten_bit,
    uint8_t* data,
    size_t size,
    FuriHalI2cBegin begin,
    FuriHalI2cEnd end,
    FuriHalCortexTimer timer);

/** Perform I2C tx and rx transfers
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      address  I2C slave address
 * @param      tx_data  pointer to tx data buffer
 * @param      tx_size  size of tx data buffer
 * @param      rx_data  pointer to rx data buffer
 * @param      rx_size  size of rx data buffer
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_trx(
    FuriHalI2cBusHandle* handle,
    const uint8_t address,
    const uint8_t* tx_data,
    size_t tx_size,
    uint8_t* rx_data,
    size_t rx_size,
    uint32_t timeout);

/** Check if I2C device presents on bus
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      timeout  timeout in milliseconds
 *
 * @return     true if device present and is ready, false otherwise
 */
bool furi_hal_i2c_is_device_ready(FuriHalI2cBusHandle* handle, uint8_t i2c_addr, uint32_t timeout);

/** Perform I2C device register read (8-bit)
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      reg_addr register address
 * @param      data     pointer to register value
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_read_reg_8(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t reg_addr,
    uint8_t* data,
    uint32_t timeout);

/** Perform I2C device register read (16-bit)
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      reg_addr register address
 * @param      data     pointer to register value
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_read_reg_16(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t reg_addr,
    uint16_t* data,
    uint32_t timeout);

/** Perform I2C device memory read
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      mem_addr memory start address
 * @param      data     pointer to data buffer
 * @param      len      size of data buffer
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_read_mem(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t mem_addr,
    uint8_t* data,
    size_t len,
    uint32_t timeout);

/** Perform I2C device register write (8-bit)
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      reg_addr register address
 * @param      data     register value
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_write_reg_8(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t reg_addr,
    uint8_t data,
    uint32_t timeout);

/** Perform I2C device register write (16-bit)
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      reg_addr register address
 * @param      data     register value
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_write_reg_16(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t reg_addr,
    uint16_t data,
    uint32_t timeout);

/** Perform I2C device memory
 *
 * @param      handle   pointer to FuriHalI2cBusHandle instance
 * @param      i2c_addr I2C slave address
 * @param      mem_addr memory start address
 * @param      data     pointer to data buffer
 * @param      len      size of data buffer
 * @param      timeout  timeout in milliseconds
 *
 * @return     true on successful transfer, false otherwise
 */
bool furi_hal_i2c_write_mem(
    FuriHalI2cBusHandle* handle,
    uint8_t i2c_addr,
    uint8_t mem_addr,
    uint8_t const* data,
    size_t len,
    uint32_t timeout);

#ifdef __cplusplus
}
#endif
