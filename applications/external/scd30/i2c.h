#pragma once
// Credit: https://github.com/NaejEL/flipperzero-i2ctools

#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>

#define SCL gpio_ext_pc0
#define SDA gpio_ext_pc1

#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 10

#define MAX_MESSAGE_SIZE 128
#define MAX_RECORDS 128

struct I2C {
    enum class BusState { Free, Started };

    struct Frame {
        uint8_t data[MAX_MESSAGE_SIZE];
        bool ack[MAX_MESSAGE_SIZE];
        uint8_t bitIndex;
        uint8_t dataIndex;
    };

    bool started = false;
    bool first = true;
    BusState state = BusState::Free;
    Frame frames[MAX_RECORDS];
    uint8_t frameIndex = 0;
    uint8_t menuIndex = 0;
    uint8_t rowIndex = 0;
    uint8_t address = 0;

    I2C(uint8_t address_);
    ~I2C();

    void clearBuffers();
    void startInterrupts();

    bool write(const uint8_t* data, uint8_t size);
    bool read(uint8_t* buffer, uint8_t size);
    bool writeThenRead(const uint8_t* tx_data, uint8_t tx_size, uint8_t* rx_data, uint8_t rx_size);

    bool readReg8(uint8_t reg_addr, uint8_t& out);
    bool readReg16(uint8_t reg_addr, uint16_t& out);
    bool writeReg8(uint8_t reg_addr, uint8_t data);
    bool writeReg16(uint8_t reg_addr, uint16_t data);

    void acquire();
    void release();

    static int count;
    static void stopInterrupts();
};
