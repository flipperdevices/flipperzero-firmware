#pragma once

/**
 * @file softio.h
 * Software (bit-banged) SPI implementation. Master-only. Supports all 4 modes
 * with clock rates of up to 200kHz.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <furi.h>

/**
 * Software SPI bus configuration
 */
typedef struct {
    const GpioPin* miso;
    const GpioPin* mosi;
    const GpioPin* sck;
    const GpioPin* cs;
    uint32_t clk_fq_khz;
    uint8_t clk_polarity : 1;
    uint8_t clk_phase    : 1;
} SoftIoSpiConfig;

/**
 * @brief Initializes bus pins, brings the CS low
 */
void softio_spi_acquire(SoftIoSpiConfig* config);

/**
 * @brief Brings the CS high, resets bus pins
 */
void softio_spi_release(SoftIoSpiConfig* config);

/**
 * @brief Simultaneously transmits and receives a buffer on the software SPI bus
 * @param [in] config Software SPI bus configuration
 * @param [in] tx_buffer Buffer to transmit. May be NULL if transmission is not required.
 * @param [in] rx_buffer Buffer to receive data into. May be NULL if reception is not required.
 * @param size Buffer length (both buffers must be of the same size)
 * @param timeout Timeout in ticks. Transaction will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_trx(
    SoftIoSpiConfig* config,
    const uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout);

/**
 * @brief Transmits a buffer on the software SPI bus
 * @param [in] config Software SPI bus configuration
 * @param [in] buffer Buffer to transmit
 * @param size Buffer length
 * @param timeout Timeout in ticks. Transmission will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_tx(SoftIoSpiConfig* config, const uint8_t* buffer, size_t size, uint32_t timeout);

/**
 * @brief Receives a buffer from the software SPI bus
 * @param [in] config Software SPI bus configuration
 * @param [in] buffer Buffer to receive into
 * @param size Buffer length
 * @param timeout Timeout in ticks. Reception will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_rx(SoftIoSpiConfig* config, uint8_t* buffer, size_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif
