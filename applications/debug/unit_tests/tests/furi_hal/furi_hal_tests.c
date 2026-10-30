#include <stdint.h>
#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <lp5562_reg.h>
#include "../test.h" // IWYU pragma: keep

#define DATA_SIZE             4
#define EEPROM_ADDRESS        0b10101000
#define EEPROM_ADDRESS_HIGH   (EEPROM_ADDRESS | 0b10)
#define EEPROM_SIZE           512
#define EEPROM_PAGE_SIZE      16
#define EEPROM_WRITE_DELAY_MS 6
#define SERIAL_TEST_BAUD_RATE 115200
#define SERIAL_TEST_DMA_CHUNK 16

static FuriHalSerialHandle* furi_hal_serial_test_handles[FuriHalSerialIdMax] = {0};

typedef struct {
    FuriHalSerialHandle* expected_handle;
    volatile bool handle_mismatch;
} FuriHalSerialTestCallbackContext;

static void furi_hal_i2c_int_setup(void) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_power);
}

static void furi_hal_i2c_int_teardown(void) {
    furi_hal_i2c_release(&furi_hal_i2c_handle_power);
}

static void furi_hal_i2c_ext_setup(void) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
}

static void furi_hal_i2c_ext_teardown(void) {
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

static void furi_hal_serial_test_setup(void) {
    furi_hal_serial_test_handles[FuriHalSerialIdUsart] =
        furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_hal_serial_test_handles[FuriHalSerialIdLpuart] =
        furi_hal_serial_control_acquire(FuriHalSerialIdLpuart);

    if(furi_hal_serial_test_handles[FuriHalSerialIdUsart] &&
       furi_hal_serial_test_handles[FuriHalSerialIdLpuart]) {
        for(size_t i = 0; i < FuriHalSerialIdMax; i++) {
            furi_hal_serial_init(furi_hal_serial_test_handles[i], SERIAL_TEST_BAUD_RATE);
        }
    }
}

static void furi_hal_serial_test_teardown(void) {
    for(size_t i = 0; i < FuriHalSerialIdMax; i++) {
        if(!furi_hal_serial_test_handles[i]) {
            continue;
        }

        furi_hal_serial_control_release(furi_hal_serial_test_handles[i]);
        furi_hal_serial_test_handles[i] = NULL;
    }
}

static void furi_hal_serial_test_shared_async_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    FuriHalSerialTestCallbackContext* callback_context = context;

    if(handle != callback_context->expected_handle) {
        callback_context->handle_mismatch = true;
    }

    if(event & FuriHalSerialRxEventData) {
        while(furi_hal_serial_async_rx_available(handle)) {
            (void)furi_hal_serial_async_rx(handle);
        }
    }
}

static void furi_hal_serial_test_shared_dma_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    size_t data_len,
    void* context) {
    FuriHalSerialTestCallbackContext* callback_context = context;
    UNUSED(data_len);

    if(handle != callback_context->expected_handle) {
        callback_context->handle_mismatch = true;
    }

    if(event & FuriHalSerialRxEventData) {
        uint8_t data[SERIAL_TEST_DMA_CHUNK];

        while(furi_hal_serial_dma_rx(handle, data, sizeof(data)) > 0) {
        }
    }
}

MU_TEST(furi_hal_i2c_int_1b) {
    bool ret = false;
    uint8_t data_one = 0;

    // 1 byte: read, write, read
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "0 read_reg_8 failed");
    mu_assert(data_one != 0, "0 invalid data");
    ret = furi_hal_i2c_write_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "1 write_reg_8 failed");
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "2 read_reg_8 failed");
    mu_assert(data_one != 0, "2 invalid data");
}

MU_TEST(furi_hal_i2c_int_3b) {
    bool ret = false;
    uint8_t data_many[DATA_SIZE] = {0};

    // 3 byte: read, write, read
    data_many[0] = LP5562_CHANNEL_BLUE_CURRENT_REGISTER;
    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, 1, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "3 tx failed");
    ret = furi_hal_i2c_rx(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        data_many + 1,
        DATA_SIZE - 1,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "4 rx failed");
    for(size_t i = 0; i < DATA_SIZE; i++)
        mu_assert(data_many[i] != 0, "4 invalid data_many");

    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, DATA_SIZE, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "5 tx failed");

    ret = furi_hal_i2c_tx(
        &furi_hal_i2c_handle_power, LP5562_ADDRESS, data_many, 1, LP5562_I2C_TIMEOUT);
    mu_assert(ret, "6 tx failed");
    ret = furi_hal_i2c_rx(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        data_many + 1,
        DATA_SIZE - 1,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "7 rx failed");
    for(size_t i = 0; i < DATA_SIZE; i++)
        mu_assert(data_many[i] != 0, "7 invalid data_many");
}

MU_TEST(furi_hal_i2c_int_1b_fail) {
    bool ret = false;
    uint8_t data_one = 0;

    // 1 byte: fail, read, fail, write, fail, read
    data_one = 0;
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS + 0x10,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(!ret, "8 read_reg_8 failed");
    mu_assert(data_one == 0, "8 invalid data");
    ret = furi_hal_i2c_read_reg_8(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        LP5562_CHANNEL_BLUE_CURRENT_REGISTER,
        &data_one,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "9 read_reg_8 failed");
    mu_assert(data_one != 0, "9 invalid data");
}

MU_TEST(furi_hal_i2c_int_ext_3b) {
    bool ret = false;
    uint8_t data_many[DATA_SIZE] = {0};

    // 3 byte: read
    data_many[0] = LP5562_CHANNEL_BLUE_CURRENT_REGISTER;
    ret = furi_hal_i2c_tx_ext(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        false,
        data_many,
        1,
        FuriHalI2cBeginStart,
        FuriHalI2cEndAwaitRestart,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "3 tx failed");

    // Send a RESTART condition, then read the 3 bytes one after the other
    ret = furi_hal_i2c_rx_ext(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        false,
        data_many + 1,
        1,
        FuriHalI2cBeginRestart,
        FuriHalI2cEndPause,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "4 rx failed");
    mu_assert(data_many[1] != 0, "4 invalid data");
    ret = furi_hal_i2c_rx_ext(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        false,
        data_many + 2,
        1,
        FuriHalI2cBeginResume,
        FuriHalI2cEndPause,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "5 rx failed");
    mu_assert(data_many[2] != 0, "5 invalid data");
    ret = furi_hal_i2c_rx_ext(
        &furi_hal_i2c_handle_power,
        LP5562_ADDRESS,
        false,
        data_many + 3,
        1,
        FuriHalI2cBeginResume,
        FuriHalI2cEndStop,
        LP5562_I2C_TIMEOUT);
    mu_assert(ret, "6 rx failed");
    mu_assert(data_many[3] != 0, "6 invalid data");
}

MU_TEST(furi_hal_i2c_ext_eeprom) {
    if(!furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, EEPROM_ADDRESS, 100)) {
        printf("no device connected, skipping\r\n");
        return;
    }

    bool ret = false;
    uint8_t buffer[EEPROM_SIZE] = {0};

    for(size_t page = 0; page < (EEPROM_SIZE / EEPROM_PAGE_SIZE); ++page) {
        // Fill page buffer
        for(size_t page_byte = 0; page_byte < EEPROM_PAGE_SIZE; ++page_byte) {
            // Each byte is its position in the EEPROM modulo 256
            uint8_t byte = ((page * EEPROM_PAGE_SIZE) + page_byte) % 256;

            buffer[page_byte] = byte;
        }

        uint8_t address = (page < 16) ? EEPROM_ADDRESS : EEPROM_ADDRESS_HIGH;

        ret = furi_hal_i2c_write_mem(
            &furi_hal_i2c_handle_external,
            address,
            page * EEPROM_PAGE_SIZE,
            buffer,
            EEPROM_PAGE_SIZE,
            20);

        mu_assert(ret, "EEPROM write failed");
        furi_delay_ms(EEPROM_WRITE_DELAY_MS);
    }

    ret = furi_hal_i2c_read_mem(
        &furi_hal_i2c_handle_external, EEPROM_ADDRESS, 0, buffer, EEPROM_SIZE, 100);

    mu_assert(ret, "EEPROM read failed");

    for(size_t pos = 0; pos < EEPROM_SIZE; ++pos) {
        mu_assert_int_eq(pos % 256, buffer[pos]);
    }
}

// Registration-only regression coverage for both async and DMA RX start paths:
// the second start call used to abort before either port received any traffic.
MU_TEST(furi_hal_serial_async_rx_shared_callback_registration) {
    FuriHalSerialHandle* usart_handle = furi_hal_serial_test_handles[FuriHalSerialIdUsart];
    FuriHalSerialHandle* lpuart_handle = furi_hal_serial_test_handles[FuriHalSerialIdLpuart];

    mu_assert(
        usart_handle && lpuart_handle,
        "Shared async callback test requires both USART and LPUART handles");

    FuriHalSerialTestCallbackContext usart_context = {
        .expected_handle = usart_handle,
    };
    FuriHalSerialTestCallbackContext lpuart_context = {
        .expected_handle = lpuart_handle,
    };

    furi_hal_serial_async_rx_start(
        usart_handle, furi_hal_serial_test_shared_async_callback, &usart_context, false);
    furi_hal_serial_async_rx_start(
        lpuart_handle, furi_hal_serial_test_shared_async_callback, &lpuart_context, false);

    furi_hal_serial_async_rx_stop(usart_handle);
    furi_hal_serial_async_rx_stop(lpuart_handle);

    mu_assert(!usart_context.handle_mismatch, "USART async callback got wrong handle");
    mu_assert(!lpuart_context.handle_mismatch, "LPUART async callback got wrong handle");
}

MU_TEST(furi_hal_serial_dma_rx_shared_callback_registration) {
    FuriHalSerialHandle* usart_handle = furi_hal_serial_test_handles[FuriHalSerialIdUsart];
    FuriHalSerialHandle* lpuart_handle = furi_hal_serial_test_handles[FuriHalSerialIdLpuart];

    mu_assert(
        usart_handle && lpuart_handle,
        "Shared DMA callback test requires both USART and LPUART handles");

    FuriHalSerialTestCallbackContext usart_context = {
        .expected_handle = usart_handle,
    };
    FuriHalSerialTestCallbackContext lpuart_context = {
        .expected_handle = lpuart_handle,
    };

    furi_hal_serial_dma_rx_start(
        usart_handle, furi_hal_serial_test_shared_dma_callback, &usart_context, false);
    furi_hal_serial_dma_rx_start(
        lpuart_handle, furi_hal_serial_test_shared_dma_callback, &lpuart_context, false);

    furi_hal_serial_dma_rx_stop(usart_handle);
    furi_hal_serial_dma_rx_stop(lpuart_handle);

    mu_assert(!usart_context.handle_mismatch, "USART DMA callback got wrong handle");
    mu_assert(!lpuart_context.handle_mismatch, "LPUART DMA callback got wrong handle");
}

MU_TEST_SUITE(furi_hal_i2c_int_suite) {
    MU_SUITE_CONFIGURE(&furi_hal_i2c_int_setup, &furi_hal_i2c_int_teardown);
    MU_RUN_TEST(furi_hal_i2c_int_1b);
    MU_RUN_TEST(furi_hal_i2c_int_3b);
    MU_RUN_TEST(furi_hal_i2c_int_ext_3b);
    MU_RUN_TEST(furi_hal_i2c_int_1b_fail);
}

MU_TEST_SUITE(furi_hal_i2c_ext_suite) {
    MU_SUITE_CONFIGURE(&furi_hal_i2c_ext_setup, &furi_hal_i2c_ext_teardown);
    MU_RUN_TEST(furi_hal_i2c_ext_eeprom);
}

MU_TEST_SUITE(furi_hal_serial_suite) {
    MU_SUITE_CONFIGURE(&furi_hal_serial_test_setup, &furi_hal_serial_test_teardown);
    MU_RUN_TEST(furi_hal_serial_async_rx_shared_callback_registration);
    MU_RUN_TEST(furi_hal_serial_dma_rx_shared_callback_registration);
}

int run_minunit_test_furi_hal(void) {
    MU_RUN_SUITE(furi_hal_i2c_int_suite);
    MU_RUN_SUITE(furi_hal_i2c_ext_suite);
    MU_RUN_SUITE(furi_hal_serial_suite);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_furi_hal)
