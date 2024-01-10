/**
 * @file  : sgp30.h
 * @brief : (GY-)SGP30 Gas Sensor Module Header
 *
 * Header file containing declarations for the SGP30 module.
 * The Gas Sensor module provides functionality to measure potentially 
 * harmful gas levels in the air.
 * 
 * https://www.mouser.com/pdfdocs/Sensirion_Gas_Sensors_SGP30_Datasheet_EN-1148053.pdf
 *
 * @author Ian S. Cohee
 */
#pragma once

#include <furi.h>
#include <furi_hal_i2c.h>

#include <stdlib.h>

/**
 * @brief I2C addresses for this device.
*/
#define SGP30_I2C_ADDR UINT8_C(0x58)
#define SGP30_I2C_ADDR_READ UINT8_C((0x58 << 1) | 0x01)
#define SGP30_I2C_ADDR_WRITE UINT8_C((0x58 << 1) | 0x00)

/**
 * @brief Misc Constants
*/
#define SGP30_FEATURESET UINT16_C(0x0020)
#define SGP30_CRC8_POLYNOMIAL UINT8_C(0x31)
#define SGP30_CRC8_INIT UINT8_C(0xFF)
#define SGP30_WORD_LEN UINT8_C(2)
#define SGP30_DEFAULT_TIMEOUT UINT8_C(0X0A)

/**
 * @brief Get Serial ID
*/
#define SPG30_GET_SERIAL_ID_CMD UINT16_C(0x3682)
#define SPG30_GET_SERIAL_ID_CMD_LEN UINT8_C(0x02)
#define SGP30_GET_SERIAL_ID_RESP_LEN UINT8_C(0x09)

/**
 * @brief Init_air_quality
*/
#define SGP30_INIT_AIR_QUALITY_CMD UINT16_C(0x2003)
#define SGP30_INIT_AIR_QUALITY_CMD_LEN UINT8_C(0x02)
#define SGP30_INIT_AIR_QUALITY_MAX_MS SGP30_DEFAULT_TIMEOUT

/**
 * @brief Measure_air_quality
*/
#define SGP30_MEASURE_AIR_QUALITY_CMD UINT16_C(0x2008)
#define SGP30_MEASURE_AIR_QUALITY_CMD_LEN UINT8_C(0x02)
#define SGP30_MEASURE_AIR_QUALITY_RESP_LEN UINT8_C(0x06)
#define SGP30_MEASURE_AIR_QUALITY_MAX_MS UINT8_C(0x0C)

/**
 * @brief Get_baseline
*/
#define SGP30_GET_BASELINE_CMD UINT16_C(0x2015)
#define SGP30_GET_BASELINE_CMD_LEN UINT8_C(0x02)
#define SGP30_GET_BASELINE_CMD_BYTE_1 UINT8_C(0x20)
#define SGP30_GET_BASELINE_CMD_BYTE_2 UINT8_C(0x15)
#define SGP30_GET_BASELINE_CMD_RESP_LEN UINT8_C(0x06)
#define SGP30_GET_BASELINE_CMD_MAX_MS SGP30_DEFAULT_TIMEOUT

/**
 * @brief Measure_test
*/
#define SGP30_MEASURE_TEST_CMD UINT16_C(0x2032)
#define SGP30_MEASURE_TEST_CMD_LEN UINT8_C(0x02)
#define SGP30_MEASURE_TEST_RESP_LEN UINT8_C(0x03)
#define SGP30_MEASURE_TEST_MAX_MS UINT8_C(0xDC)
#define SGP30_MEASURE_TEST_PASS_VAL UINT16_C(0xD400)

/**
 * @brief Sgp30 instance definition
*/
typedef struct Sgp30 {
    FuriHalI2cBusHandle* i2c;
    uint64_t chip_id;
    uint8_t i2c_addr;
    uint8_t i2c_addr_read;
    uint8_t i2c_addr_write;
} Sgp30;

/**
 * @brief A type for returning CO2 and TVOC readings.
*/
typedef struct Sgp30Readings {
    uint64_t raw;
    uint32_t co2_tvoc;
    uint16_t co2;
    uint16_t tvoc;
} Sgp30Readings;

/**
 * @brief Allocate a Sgp30 instance.
 *
 * @return Pointer to Sgp30 instance.
 */
Sgp30* sgp30_alloc(void);

/**
 * @brief Initialize Sgp30 instance.
 * Read Chip ID and initialize measurement data.
 * 
 * @param[in]  sgp30 : Pointer to a Sgp30 instance
 * 
 * @return Void.
*/
void sgp30_init(Sgp30* sgp30);

/**
 * @brief Free a Sgp30 instance.
 *
 * @param[in]  sgp30 : Pointer to a Sgp30 instance
 *
 * @return Void.
 */
void sgp30_free(Sgp30* sgp30);

/**
 * @brief Read data from SGP30 instance using I2C.
 * 
 * CRC checksum is not calculated for bytes read.
 *
 * @param[in]   sgp30    : Pointer to a Sgp30 instance
 * @param[out]  data     : Pointer to byte array to read data into
 * @param[in]   data_len : Number of bytes to read
 * @param[in]   timeout  : Timeout in useconds
 *
 * @return True on success, false on failure.
 */
bool sgp30_read(Sgp30* sgp30, uint8_t* data, uint32_t data_len, uint32_t timeout);

/**
 * @brief Read a word (16-bit) from sensor and check the CRC (third byte).
 * 
 * sgp30_read_word only reads 24-bits: 1 16-bit word + 1 8-bit CRC
 * 
 * @param[in]   sgp30    : Pointer to Sgp30 instance.
 * @param[out]  data     : Data buffer to read into.
 * @param[in]   timeout  : Timeout in useconds
 * 
 * @return True if read succeeds and CRC matches, false on failure.
*/
bool sgp30_read_word(Sgp30* sgp30, uint8_t* data, uint32_t timeout);

/**
 * @brief Read two words (16-bit each) from sensor and check the CRCs (bytes 2 & 5).
 * 
 * @param[in]   sgp30    : Pointer to Sgp30 instance.
 * @param[out]  data     : Data buffer to read into.
 * @param[in]   timeout  : Timeout in useconds
 * 
 * @return True if read succeeds and CRC matches, false on failure.
*/
bool sgp30_read_double_word(Sgp30* sgp30, uint8_t* data, uint32_t timeout);

/**
 * @brief Write command to SGP30 instance using I2C.
 *
 * @param[in]  sgp30    : Pointer to a Sgp30 instance
 * @param[in]  command  : Pointer to byte array containing command bytes
 * @param[in]  data_len : Number of bytes to read
 * @param[in]  timeout  : Timeout in useconds
 *
 * @return True on success, false on failure.
 */
bool sgp30_write(Sgp30* sgp30, const uint8_t* command, uint32_t data_len, uint32_t timeout);

/**
 * @brief Write command to SGP30 instance and receive output using I2C.
 * A 0.5ms pause occurs between write and read, per the datasheet. 
 * 
 * This function uses a single buffer, it is the caller's responsibility to make
 * sure the buffer is big enough for MAX(command, response).
 * 
 * The command is overwritten by the response.
 *
 * @param[in]      sgp30       : Pointer to a Sgp30 instance
 * @param[in,out]  data        : Pointer to byte array containing command bytes
 * @param[in]      command_len : Length of the command being sent
 * @param[in]      data_len    : Number of bytes to read after sending the command
 * @param[in]      timeout     : Timeout in useconds
 *
 * @return True on success, false on failure.
 */
bool sgp30_trx(
    Sgp30* sgp30,
    uint8_t* data,
    uint32_t command_len,
    uint32_t data_len,
    uint32_t timeout);

/**
 * @brief Get ID from the chip.
 * Result is stored in sgp30->chip_id, and can be accessed via sgp30_get_id()
 * 
 * This implmentation was take from 
 * https://github.com/adafruit/Adafruit_SGP30/blob/master/Adafruit_SGP30.cpp
 *
 * @param[in]  sgp30 : Pointer to sgp30 instance.
 *
 * @return True on success, false on failure.
 */
bool sgp30_get_chip_id(Sgp30* sgp30);

/**
 * @brief Get ID from instance.
 *
 * @param[in]  sgp30 : Pointer to sgp30 instance.
 *
 * @return 48-bit chip ID as uint64_t.
 */
uint64_t sgp30_get_id(Sgp30* sgp30);

/**
 * @brief Initiate a self test.
 * Run a built-in test, the result of which should be 0xD400.
 * If the self test does not return this value, this function
 * return false.
 * 
 * This implmentation was take from 
 * https://github.com/adafruit/Adafruit_SGP30/blob/master/Adafruit_SGP30.cpp
 *
 * @param[in]  sgp30 : Pointer to sgp30 instance.
 *
 * @return True of test passed, false if failed.
 */
bool sgp30_measure_test(Sgp30* sgp30);

/**
 * @brief Get_baseline configuration.
 * 
 * @param[in]        sgp30 : Pointer to Sgp30 instance.
 * @param[in,out]     data : Pointer to uint8_t array to store data 
 * 
 * @return True on success, false on failure.
*/
bool sgp30_get_baseline(Sgp30* sgp30, uint8_t* data);

/**
 * @brief Measure air quality.
 * 
 * @param[in]        sgp30 : Pointer to Sgp30 instance.
 * @param[in,out] readings : Pointer to Sgp30Readings instance.
 * 
 * @return True on success, false on failure.
*/
bool sgp30_measure_air_quality(Sgp30* sgp30, Sgp30Readings* readings);

/**
 * @brief Soft reset the chip.
 * 
 * @param[in]  sgp30 : Pointer to Sgp30 instance;
 * 
 * @return Void.
*/
void sgp30_soft_reset(Sgp30* sgp30);

/**
 * @brief Generate CRC checksum for given data.
 * This device expects each command to include a CRC byte.
 * Exceptions are the word (2-byte) commands that already
 * include a 3-bit checksum.
 * 
 * This implmentation was take from 
 * https://github.com/adafruit/Adafruit_SGP30/blob/master/Adafruit_SGP30.cpp
 *
 * @param[in]  data    : Pointer to byte array to checksum.
 * @param[in]  datalen : Length of byte array to use in CRC
 *
 * @return CRC checksum byte.
 */
uint8_t sgp30_generate_crc(uint8_t* data, uint8_t datalen);