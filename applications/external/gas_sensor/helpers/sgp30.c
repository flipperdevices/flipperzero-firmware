/**
 * @file  : sgp30.c
 * @brief : GY-SGP30 Gas Sensor Module implementation
 *
 * Code file containing implementations for the GY-SGP30 module.
 * The Gas Sensor module provides functionality to measure potentially harmful
 * gas levels in the air.
 *
 * @author Ian S. Cohee
 */

#include "sgp30.h"

#define TAG "sgp30"

Sgp30* sgp30_alloc(void) {
    Sgp30* sgp30 = malloc(sizeof(Sgp30));
    furi_check(sgp30);

    sgp30->i2c = &furi_hal_i2c_handle_external;
    sgp30->i2c_addr = SGP30_I2C_ADDR;
    sgp30->i2c_addr_read = SGP30_I2C_ADDR_READ;
    sgp30->i2c_addr_write = SGP30_I2C_ADDR_WRITE;

    // chip takes a max. of 0.6ms to start after power up
    furi_delay_us(600);
    return sgp30;
}

void sgp30_init(Sgp30* sgp30) {
    FURI_LOG_D(TAG, "initializing chip");
    furi_assert(sgp30);
    if(sgp30_get_chip_id(sgp30) == false) {
        FURI_LOG_E(TAG, "failed to initialize; could not get chip id");
        return;
    }
    if(sgp30_measure_test(sgp30) == false) {
        FURI_LOG_E(TAG, "measure test failed, continuing anyway");
    }
    uint8_t data[2];
    data[0] = (uint8_t)((SGP30_INIT_AIR_QUALITY_CMD & 0xFF00) >> 8);
    data[1] = (uint8_t)(SGP30_INIT_AIR_QUALITY_CMD & 0x00FF);
    if(sgp30_write(sgp30, data, SGP30_INIT_AIR_QUALITY_CMD_LEN, SGP30_INIT_AIR_QUALITY_MAX_MS) ==
       false) {
        FURI_LOG_E(TAG, "failed to send Init_air_quality command to sgp30");
    }
}

void sgp30_free(Sgp30* sgp30) {
    furi_assert(sgp30);
    sgp30->i2c = NULL;
    free(sgp30);
}

bool sgp30_read(Sgp30* sgp30, uint8_t* data, uint32_t data_len, uint32_t timeout) {
    furi_assert(sgp30);
    furi_assert(data);
    bool status = false;
    furi_hal_i2c_acquire(sgp30->i2c);
    status = furi_hal_i2c_rx(sgp30->i2c, sgp30->i2c_addr_read, data, data_len, timeout);
    furi_hal_i2c_release(sgp30->i2c);
    return status;
}

bool sgp30_read_word(Sgp30* sgp30, uint8_t* data, uint32_t timeout) {
    furi_assert(sgp30);
    furi_assert(data);
    if(sgp30_read(sgp30, data, SGP30_WORD_LEN + 1, timeout) == false) {
        FURI_LOG_E(TAG, "read_word: read failed");
        return false;
    }
    uint8_t crc = sgp30_generate_crc(data, SGP30_WORD_LEN);
    if(crc != data[2]) {
        FURI_LOG_E(
            TAG, "read_word: CRC does not match: expected=0x%02X, received=0x%02X", crc, data[2]);
        return false;
    }
    return true;
}

bool sgp30_read_double_word(Sgp30* sgp30, uint8_t* data, uint32_t timeout) {
    furi_assert(sgp30);
    furi_assert(data);
    if(sgp30_read(sgp30, data, (SGP30_WORD_LEN * 2) + 2, timeout) == false) {
        FURI_LOG_E(TAG, "read_double_word: read failed");
        return false;
    }
    uint8_t crc = sgp30_generate_crc(data, SGP30_WORD_LEN);
    if(crc != data[2]) {
        FURI_LOG_E(
            TAG,
            "read_double_word: CRC (1) does not match: expected=0x%02X, received=0x%02X",
            crc,
            data[2]);
        return false;
    }
    crc = sgp30_generate_crc(&data[3], SGP30_WORD_LEN);
    if(crc != data[5]) {
        FURI_LOG_E(
            TAG,
            "read_double_word CRC (2) does not match: expected=0x%02X, received=0x%02X",
            crc,
            data[5]);
        return false;
    }
    return true;
}

bool sgp30_write(Sgp30* sgp30, const uint8_t* command, uint32_t data_len, uint32_t timeout) {
    furi_assert(sgp30);
    furi_assert(command);
    bool status = false;
    furi_hal_i2c_acquire(sgp30->i2c);
    status = furi_hal_i2c_tx(sgp30->i2c, sgp30->i2c_addr_write, command, data_len, timeout);
    furi_hal_i2c_release(sgp30->i2c);
    return status;
}

bool sgp30_trx(
    Sgp30* sgp30,
    uint8_t* data,
    uint32_t command_len,
    uint32_t data_len,
    uint32_t timeout) {
    furi_assert(sgp30);
    furi_assert(data);
    bool status = false;

    furi_hal_i2c_acquire(sgp30->i2c);
    if(furi_hal_i2c_tx(sgp30->i2c, sgp30->i2c_addr_write, data, command_len, timeout) == false) {
        FURI_LOG_E(TAG, "trx: failed to send command to sgp30");
        goto release_lock_exit;
    }

    memset(data, 0, data_len);

    // 0.5ms delay
    furi_delay_us(1000000);
    status = furi_hal_i2c_rx(sgp30->i2c, sgp30->i2c_addr_read, data, data_len, timeout);
    if(status == false) {
        FURI_LOG_E(TAG, "trx: failed to read response from sgp30");
    }

release_lock_exit:
    furi_hal_i2c_release(sgp30->i2c);
    return status;
}

bool sgp30_get_chip_id(Sgp30* sgp30) {
    furi_assert(sgp30);

    // response is [msb, lsb, crc, msb, lsb, crc, msb, lsb, crc]
    uint8_t data[SGP30_GET_SERIAL_ID_RESP_LEN] = {0};
    data[0] = (uint8_t)((SPG30_GET_SERIAL_ID_CMD & 0xFF00) >> 8);
    data[1] = (uint8_t)(SPG30_GET_SERIAL_ID_CMD & 0x00FF);
    if(sgp30_trx(
           sgp30,
           data,
           SPG30_GET_SERIAL_ID_CMD_LEN,
           SGP30_GET_SERIAL_ID_RESP_LEN,
           SGP30_DEFAULT_TIMEOUT) == false) {
        FURI_LOG_E(TAG, "failed to get chip id");
        return false;
    }
    uint8_t crc = sgp30_generate_crc(&data[0], 2);
    if(crc != data[2]) {
        FURI_LOG_E(
            TAG, "crc from id (1) does not match: expected=0x%02X, sent=0x%02X", crc, data[2]);
        return false;
    }
    crc = sgp30_generate_crc(&data[3], 2);
    if(crc != data[5]) {
        FURI_LOG_E(
            TAG, "crc from id (2) does not match: expected=0x%02X, sent=0x%02X", crc, data[5]);
        return false;
    }
    crc = sgp30_generate_crc(&data[6], 2);
    if(crc != data[8]) {
        FURI_LOG_E(
            TAG, "crc from id (3) does not match: expected=0x%02X, sent=0x%02X", crc, data[8]);
        return false;
    }
    FURI_LOG_D(
        TAG,
        "chip id read returned: 0x%02x%02x%02x%02x%02x%02x",
        data[0],
        data[1],
        data[3],
        data[4],
        data[6],
        data[7]);
    uint64_t new_chip_id = 0;
    for(uint32_t i = 0; i < SGP30_GET_SERIAL_ID_RESP_LEN; ++i) {
        if(i == 2 || i == 5 || i == 8) continue;
        new_chip_id |= data[i];
        new_chip_id <<= 8;
    }
    sgp30->chip_id = new_chip_id;
    return true;
}

uint64_t sgp30_get_id(Sgp30* sgp30) {
    furi_assert(sgp30);
    return sgp30->chip_id;
}

bool sgp30_measure_test(Sgp30* sgp30) {
    furi_assert(sgp30);
    uint8_t data[2] = {0};
    uint8_t resp[3] = {0};
    data[0] = (uint8_t)((SGP30_MEASURE_TEST_CMD & 0xFF00) >> 8);
    data[1] = (uint8_t)(SGP30_MEASURE_TEST_CMD & 0x00FF);

    sgp30_write(sgp30, data, SGP30_MEASURE_TEST_CMD_LEN, SGP30_DEFAULT_TIMEOUT);
    furi_delay_us(SGP30_MEASURE_TEST_MAX_MS);
    sgp30_read(sgp30, resp, SGP30_MEASURE_TEST_RESP_LEN, SGP30_DEFAULT_TIMEOUT);

    uint8_t test_byte0 = (uint8_t)((SGP30_MEASURE_TEST_PASS_VAL & 0xFF00) >> 8);
    uint8_t test_byte1 = (uint8_t)(SGP30_MEASURE_TEST_PASS_VAL & 0x00FF);
    FURI_LOG_D(TAG, "measure_test returned: 0x%02X%02x", resp[0], resp[1]);

    if(resp[0] != test_byte0 || resp[1] != test_byte1) {
        FURI_LOG_E(TAG, "measure_test: test bytes are wrong: [0x%02X, 0x%02X]", resp[0], resp[1]);
        return false;
    }
    if(resp[2] != sgp30_generate_crc(resp, SGP30_WORD_LEN)) {
        FURI_LOG_E(TAG, "measure_test: crc mismatch");
        return false;
    }
    return true;
}

bool sgp30_get_baseline(Sgp30* sgp30, uint8_t* data) {
    furi_assert(sgp30);
    data[0] = SGP30_GET_BASELINE_CMD_BYTE_1;
    data[1] = SGP30_GET_BASELINE_CMD_BYTE_2;
    if(sgp30_write(sgp30, data, SGP30_GET_BASELINE_CMD_LEN, SGP30_GET_BASELINE_CMD_MAX_MS) ==
       false) {
        FURI_LOG_E(TAG, "failed to send Get_baseline command");
        return false;
    }
    furi_delay_us(500);
    if(sgp30_read_double_word(sgp30, data, 10) == false) {
        FURI_LOG_E(TAG, "failed to read Get_baseline data");
        return false;
    }
    return true;
}

bool sgp30_measure_air_quality(Sgp30* sgp30, Sgp30Readings* readings) {
    furi_assert(sgp30);
    furi_assert(readings);
    uint8_t data[6] = {0};

    data[0] = (uint8_t)((SGP30_MEASURE_AIR_QUALITY_CMD & 0xFF00) >> 8);
    data[1] = (uint8_t)(SGP30_MEASURE_AIR_QUALITY_CMD & 0x00FF);

    if(sgp30_trx(
           sgp30,
           data,
           SGP30_MEASURE_AIR_QUALITY_CMD_LEN,
           SGP30_MEASURE_AIR_QUALITY_RESP_LEN,
           SGP30_MEASURE_AIR_QUALITY_MAX_MS) == false) {
        FURI_LOG_E(TAG, "failed to measure air quality; trx error");
        readings->co2_tvoc = 0xFFFFFFFF;
        return false;
    }
    uint8_t crc = sgp30_generate_crc(data, SGP30_WORD_LEN);
    if(crc != data[2]) {
        FURI_LOG_E(TAG, "measure_air_quality: CRC (1) does not match");
        readings->co2_tvoc = 0xFFFFFFFF;
        return false;
    }
    crc = sgp30_generate_crc(&data[3], SGP30_WORD_LEN);
    if(crc != data[5]) {
        FURI_LOG_E(TAG, "measure_air_quality: CRC (2) does not match");
        readings->co2_tvoc = 0xFFFFFFFF;
        return false;
    }
    readings->co2 = (uint16_t)(((data[0] & 0x00FF) << 8) | (data[1] & 0x00FF));
    readings->tvoc = (uint16_t)(((data[3] & 0x00FF) << 8) | (data[4] & 0x00FF));
    readings->co2_tvoc =
        (uint32_t)(((readings->co2 & 0x0000FFFF) << 16) | (readings->tvoc & 0x0000FFFF));

    return true;
}

uint8_t sgp30_generate_crc(uint8_t* data, uint8_t datalen) {
    // calculates 8-Bit checksum with given polynomial
    uint8_t crc = SGP30_CRC8_INIT;

    for(uint8_t i = 0; i < datalen; i++) {
        crc ^= data[i];
        for(uint8_t b = 0; b < 8; b++) {
            if(crc & 0x80)
                crc = (crc << 1) ^ SGP30_CRC8_POLYNOMIAL;
            else
                crc <<= 1;
        }
    }
    return crc;
}