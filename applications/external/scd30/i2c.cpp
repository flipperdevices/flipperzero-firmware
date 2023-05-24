// Credit: https://github.com/NaejEL/flipperzero-i2ctools

#include "i2c.h"
#include "scd30_logging.h"

int I2C::count = 0;

I2C::I2C(uint8_t address_)
    : address(address_) {
    furi_assert(++count == 1);
    clearBuffers();
}

I2C::~I2C() {
    --count;
    stopInterrupts();
}

void I2C::clearBuffers() {
    furi_assert(this);
    for(uint8_t i = 0; i < MAX_RECORDS; i++) {
        for(uint8_t j = 0; j < MAX_MESSAGE_SIZE; j++) {
            frames[i].ack[j] = false;
            frames[i].data[j] = 0;
        }

        frames[i].bitIndex = 0;
        frames[i].dataIndex = 0;
    }

    frameIndex = 0;
    state = BusState::Free;
    first = true;
}

void I2C::startInterrupts() {
    furi_assert(this);
    furi_hal_gpio_init(&SCL, GpioModeInterruptRise, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(
        &SCL,
        +[](void* userdata) {
            auto& i2c = *reinterpret_cast<I2C*>(userdata);
            if(i2c.state == BusState::Free) return;
            uint8_t frame = i2c.frameIndex;
            uint8_t bit = i2c.frames[frame].bitIndex;
            uint8_t data_idx = i2c.frames[frame].dataIndex;
            if(bit < 8) {
                i2c.frames[frame].data[data_idx] <<= 1;
                i2c.frames[frame].data[data_idx] |= static_cast<int>(furi_hal_gpio_read(&SDA));
                i2c.frames[frame].bitIndex++;
            } else {
                i2c.frames[frame].ack[data_idx] = !furi_hal_gpio_read(&SDA);
                i2c.frames[frame].dataIndex++;
                i2c.frames[frame].bitIndex = 0;
            }
        },
        this);

    // Add Rise and Fall Interrupt on SDA pin
    furi_hal_gpio_init(&SDA, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(
        &SDA,
        +[](void* userdata) {
            auto& i2c = *reinterpret_cast<I2C*>(userdata);

            // SCL is low, maybe clock stretching
            if(!furi_hal_gpio_read(&SCL)) return;

            if(i2c.state == BusState::Started && furi_hal_gpio_read(&SDA)) {
                // Check for stop condition: SDA rising while SCL is High
                i2c.state = BusState::Free;
            } else if(!furi_hal_gpio_read(&SDA)) {
                // Check for start condition: SDA falling while SCL is high
                i2c.state = BusState::Started;
                if(i2c.first) {
                    i2c.first = false;
                    return;
                }

                i2c.frameIndex++;
                if(MAX_RECORDS <= i2c.frameIndex) i2c.clearBuffers();
            }
        },
        this);
}

void I2C::stopInterrupts() {
    furi_hal_gpio_remove_int_callback(&SCL);
    furi_hal_gpio_remove_int_callback(&SDA);
    // Reset GPIO pins to default state
    furi_hal_gpio_init(&SCL, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&SDA, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

#define HACK_ADDR (address << 1)

bool I2C::write(const uint8_t* data, uint8_t size) {
    acquire();
    const bool success = furi_hal_i2c_tx(I2C_BUS, HACK_ADDR, data, size, I2C_TIMEOUT);
    if(!success) ERROR("write: tx failed");
    release();
    return success;
}

bool I2C::read(uint8_t* buffer, uint8_t size) {
    acquire();
    const bool success = furi_hal_i2c_rx(I2C_BUS, HACK_ADDR, buffer, size, I2C_TIMEOUT);
    release();
    return success;
}

bool I2C::writeThenRead(const uint8_t* tx_data, uint8_t tx_size, uint8_t* rx_data, uint8_t rx_size) {
    acquire();
    const bool success =
        furi_hal_i2c_trx(I2C_BUS, HACK_ADDR, tx_data, tx_size, rx_data, rx_size, I2C_TIMEOUT);
    if(!success) ERROR("wtr: trx failed");
    release();
    return success;
}

bool I2C::readReg8(uint8_t reg_addr, uint8_t& out) {
    acquire();
    const bool success = furi_hal_i2c_read_reg_8(I2C_BUS, HACK_ADDR, reg_addr, &out, I2C_TIMEOUT);
    release();
    return success;
}

bool I2C::readReg16(uint8_t reg_addr, uint16_t& out) {
    acquire();
    const bool success = furi_hal_i2c_read_reg_16(I2C_BUS, HACK_ADDR, reg_addr, &out, I2C_TIMEOUT);
    release();
    return success;
}

bool I2C::writeReg8(uint8_t reg_addr, uint8_t data) {
    acquire();
    const bool success = furi_hal_i2c_write_reg_8(I2C_BUS, HACK_ADDR, reg_addr, data, I2C_TIMEOUT);
    release();
    return success;
}

bool I2C::writeReg16(uint8_t reg_addr, uint16_t data) {
    acquire();
    const bool success =
        furi_hal_i2c_write_reg_16(I2C_BUS, HACK_ADDR, reg_addr, data, I2C_TIMEOUT);
    release();
    return success;
}

void I2C::acquire() {
    furi_hal_i2c_acquire(I2C_BUS);
}

void I2C::release() {
    furi_hal_i2c_release(I2C_BUS);
}
