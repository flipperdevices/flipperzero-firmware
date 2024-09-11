#pragma once

/**
 * @file softio_spi.h
 * Software (bit-banged) SPI implementation. Master-only. Supports all 4 modes
 * with clock rates of up to 200kHz.
 * 
 * @example
 * ```c
 * // initialize
 * SoftIoSpiBusConfig bus_cfg = {
 *     .miso = &gpio_ext_pa6,
 *     .mosi = &gpio_ext_pa7,
 *     .sck = &gpio_ext_pb3,
 *     .clk_polarity = 0,
 * };
 * SoftIoSpiSlaveConfig dev_cfg = {
 *     .cs = &gpio_ext_pa4,
 *     .clk_fq_khz = 200,
 *     .clk_phase = 0,
 * };
 * SoftIoSpiBus* bus = softio_spi_alloc(&bus_cfg);
 * SoftIoSpiSlave* device = softio_spi_attach_slave(bus, &dev_cfg);
 * softio_spi_init(bus);
 * 
 * // transmit
 * uint8_t buffer[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x55, 0xAA};
 * softio_spi_acquire(device);
 * softio_spi_tx(device, buffer, sizeof(buffer), FuriWaitForever);
 * softio_spi_release(device);
 * 
 * // deinitialize
 * softio_spi_deinit(bus);
 * softio_spi_detach_slave(bus, device);
 * softio_spi_free(bus);
 * ```
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <furi.h>

/**
 * @brief Opaque software SPI bus handle
 */
typedef struct SoftIoSpiBus SoftIoSpiBus;

/**
 * @brief Opaque software SPI bus device handle
 */
typedef struct SoftIoSpiSlave SoftIoSpiSlave;

/**
 * @brief Software SPI bus configuration
 */
typedef struct {
    const GpioPin* miso;
    const GpioPin* mosi;
    const GpioPin* sck;
    uint8_t clk_polarity : 1;
} SoftIoSpiBusConfig;

/**
 * @brief Software SPI bus device configuration
 */
typedef struct {
    const GpioPin* cs;
    uint32_t clk_fq_khz;
    uint8_t clk_phase : 1;
} SoftIoSpiSlaveConfig;

/**
 * @brief Allocates a software SPI bus with the specified configuration
 * @param [in] config Pointer to the configuration structure. Does not have to remain valid after the function exits.
 */
SoftIoSpiBus* softio_spi_alloc(SoftIoSpiBusConfig* config);

/**
 * @brief Deallocates a software SPI bus
 * @param [in] bus Software SPI bus
 */
void softio_spi_free(SoftIoSpiBus* bus);

/**
 * @brief Registers a slave with a software SPI bus
 * @param [in] bus Software SPI bus
 * @param [in] config Pointer to the configuration structure. Does not have to remain valid after the function exits.
 */
SoftIoSpiSlave* softio_spi_attach_slave(SoftIoSpiBus* bus, SoftIoSpiSlaveConfig* config);

/**
 * @brief Detaches a CS pin from a software SPI bus
 * @param [in] bus Software SPI bus
 * @param [in] device Software SPI bus device
 */
void softio_spi_detach_slave(SoftIoSpiBus* bus, SoftIoSpiSlave* device);

/**
 * @brief Initializes bus pins: MOSI, MISO and SCK and all CS pins
 * @param [in] bus Software SPI bus
 */
void softio_spi_init(SoftIoSpiBus* bus);

/**
 * @brief Deinitializes bus pins: MOSI, MISO and SCK and all CS pins
 * @param [in] bus Software SPI bus
 */
void softio_spi_deinit(SoftIoSpiBus* bus);

/**
 * @brief Brings the CS pin associated with a slave low
 * @param [in] device Software SPI bus device
 */
void softio_spi_acquire(SoftIoSpiSlave* device);

/**
 * @brief Brings the CS pin associated with a slave high
 * @param [in] device Software SPI bus device
 */
void softio_spi_release(SoftIoSpiSlave* device);

/**
 * @brief Simultaneously transmits and receives a buffer on the software SPI bus
 * @param [in] device Software SPI bus device
 * @param [in] tx_buffer Buffer to transmit. May be NULL if transmission is not required.
 * @param [in] rx_buffer Buffer to receive data into. May be NULL if reception is not required.
 * @param size Buffer length (both buffers must be of the same size)
 * @param timeout Timeout in ticks. Transaction will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_trx(
    SoftIoSpiSlave* device,
    const uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout);

/**
 * @brief Transmits a buffer on the software SPI bus
 * @param [in] device Software SPI bus device
 * @param [in] buffer Buffer to transmit
 * @param size Buffer length
 * @param timeout Timeout in ticks. Transmission will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_tx(SoftIoSpiSlave* device, const uint8_t* buffer, size_t size, uint32_t timeout);

/**
 * @brief Receives a buffer from the software SPI bus
 * @param [in] device Software SPI bus device
 * @param [in] buffer Buffer to receive into
 * @param size Buffer length
 * @param timeout Timeout in ticks. Reception will be interrupted abruptly if this timeout is reached.
 */
void softio_spi_rx(SoftIoSpiSlave* device, uint8_t* buffer, size_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif
