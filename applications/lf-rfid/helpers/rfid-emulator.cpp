#include "rfid-emulator.h"

constexpr uint32_t em_big = 520;
constexpr uint32_t em_small = 260;

constexpr bool hi = true;
constexpr bool low = false;

void RfidEmulator::do_gpio(bool data, float time) {
    gpio_write(pull_pin_record, data);
    delay_us(time);
}

void RfidEmulator::do_emarine(void) {
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_big);
    do_gpio(low, em_big);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_big);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_big);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
    do_gpio(hi, em_small);
    do_gpio(low, em_small);
}

void RfidEmulator::do_hid(void) {
    // packet preamble
    hid_0();
    hid_0();
    hid_0();
    hid_1();
    hid_1();
    hid_1();
    hid_0();
    hid_1();

    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();

    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_1();
    hid_0();
    hid_0();
    hid_1();

    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();

    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();

    hid_0();
    hid_1();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();

    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_0();
    hid_1();
    hid_1();
    hid_0();

    hid_1();
    hid_0();
    hid_0();
    hid_1();
    hid_1();
    hid_0();
    hid_1();
    hid_0();

    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();

    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_0();
    hid_1();

    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_0();
    hid_1();

    hid_1();
    hid_0();
    hid_1();
    hid_0();
    hid_0();
    hid_1();
    hid_1();
    hid_0();
}

constexpr uint32_t hid_hi = 79 / 2;
constexpr uint32_t hid_low = 62 / 2;
static float hid_tune_hi = 0;
static float hid_tune_low = 0;

void RfidEmulator::hid_0(void) {
    for(uint8_t i = 0; i < 6; i++) {
        do_gpio(low, (float)hid_low + hid_tune_low);
        do_gpio(hi, (float)hid_low + hid_tune_low);
    }
}

void RfidEmulator::hid_1(void) {
    for(uint8_t i = 0; i < 5; i++) {
        do_gpio(low, (float)hid_hi + hid_tune_hi);
        do_gpio(hi, (float)hid_hi + hid_tune_hi);
    }
}

void prepare_data(uint32_t ID, uint32_t VENDOR, uint8_t* data) {
    uint8_t value[10];

    // vendor rows (4 bit in a row)
    value[0] = (VENDOR >> 4) & 0xF;
    value[1] = VENDOR & 0xF;

    const uint8_t ROW_SIZE = 4;
    const uint8_t HEADER_SIZE = 9;

    // ID rows (4 bit in a row)
    for(int i = 0; i < 8; i++) {
        value[i + 2] = (ID >> (28 - i * ROW_SIZE)) & 0xF;
    }

    for(uint8_t i = 0; i < HEADER_SIZE; i++) {
        data[i] = 1; // header
    }

    for(uint8_t i = 0; i < 10; i++) { // data
        for(uint8_t j = 0; j < ROW_SIZE; j++) {
            data[HEADER_SIZE + i * (ROW_SIZE + 1) + j] = (value[i] >> ((ROW_SIZE - 1) - j)) & 1;
        }

        // row parity
        data[HEADER_SIZE + i * (ROW_SIZE + 1) + ROW_SIZE] =
            (data[HEADER_SIZE + i * (ROW_SIZE + 1) + 0] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 1] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 2] +
             data[HEADER_SIZE + i * (ROW_SIZE + 1) + 3]) %
            2;
    }

    for(uint8_t i = 0; i < ROW_SIZE; i++) { //checksum
        uint8_t checksum = 0;
        for(uint8_t j = 0; j < 10; j++) {
            checksum += data[HEADER_SIZE + i + j * (ROW_SIZE + 1)];
        }
        data[i + 59] = checksum % 2;
    }

    data[63] = 0; // stop bit
}

void em4100_emulation(uint8_t* data, GpioPin* pin) {
    taskENTER_CRITICAL();
    gpio_write(pin, true);

    for(uint8_t i = 0; i < 8; i++) {
        for(uint8_t j = 0; j < 64; j++) {
            delay_us(260);
            gpio_write(pin, data[j]);
            delay_us(260);
            gpio_write(pin, !data[j]);
        }
    }

    gpio_write(pin, false);
    taskEXIT_CRITICAL();
}

void RfidEmulator::do_emarine_old(void) {
    uint8_t raw_data[64];
    for(size_t i = 0; i < 64; i++) {
        raw_data[i] = 0;
    }

    uint8_t customer_id = 00;
    uint32_t em_data = 4378151;

    prepare_data(em_data, customer_id, raw_data);
    em4100_emulation(raw_data, pull_pin_record);
}

RfidEmulator::RfidEmulator() {
}

void RfidEmulator::start() {
    gpio_init(out_pin_record, GpioModeOutputPushPull);
    gpio_init(pull_pin_record, GpioModeOutputPushPull);

    gpio_write(out_pin_record, false);
    gpio_write(pull_pin_record, false);

    // pulldown iBtn pin to prevent interference from ibutton
    gpio_init(&ibutton_gpio, GpioModeOutputOpenDrain);
    gpio_write(&ibutton_gpio, false);
}

void RfidEmulator::stop() {
    gpio_write(out_pin_record, false);
    gpio_write(pull_pin_record, false);
    gpio_init(pull_pin_record, GpioModeInterruptRise);
}

void RfidEmulator::emulate(LfrfidKeyType type, uint8_t* data, uint8_t data_size) {
    __disable_irq();
    start();

    switch(type) {
    case LfrfidKeyType::KeyEmarine:
        for(uint8_t i = 0; i < 8; i++) {
            do_emarine();
        }
        break;

    case LfrfidKeyType::KeyHID:
        const uint8_t hid_steps = 40;

        hid_tune_hi = -(hid_steps / 2);
        hid_tune_hi = hid_tune_hi / 10;

        for(uint8_t hid_step_1 = 0; hid_step_1 < hid_steps; hid_step_1++) {
            for(uint8_t hid_step_2 = 0; hid_step_2 < hid_steps; hid_step_2++) {
                hid_tune_low = -(hid_steps / 2);
                hid_tune_low = hid_tune_low / 10;

                for(uint8_t i = 0; i < 8; i++) {
                    do_hid();
                }

                hid_tune_low += 0.1f;
            }
            hid_tune_hi += 0.1f;
        }

        break;
    }

    stop();
    __enable_irq();
}
