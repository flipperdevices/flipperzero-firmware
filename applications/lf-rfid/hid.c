#include <furi.h>
#include <api-hal.h>

void hid_prepare_data(uint8_t facility_code, uint16_t card_no, uint8_t* data) {
    for(size_t i = 0; i < 96; i++) {
        data[i] = 0;
    }


    uint64_t card_format = 1;
    uint8_t oem_code = 1;

    uint16_t op_bits = card_no & 0b111111111111;
    uint16_t ep_bits = (card_no >> 12 & 0b1111) | (uint16_t)facility_code << 4;

    uint8_t op = 1;
    for(uint8_t i = 0; i < 12; i++) {
        op ^= (op_bits >> i) & 1;
    }

    uint8_t ep = 0;
    for(uint8_t i = 0; i < 12; i++) {
        ep ^= (ep_bits >> i) & 1;
    }

    uint64_t result = 0;
    result |= ((uint64_t)op & 1) << 0;
    result |= ((uint64_t)card_no & 0xFFFF) << 1;
    result |= ((uint64_t)facility_code & 0xFF) << 17;
    result |= ((uint64_t)ep & 1) << 25;
    result |= ((uint64_t)card_format & 0b11111111111) << 26;
    result |= ((uint64_t)oem_code & 0b1111111) << 37;

    // write a preamble
    data[3] = 1;
    data[4] = 1;
    data[5] = 1;
    data[7] = 1;

    // oem code
    // data[37] = 1;

    size_t out_idx = 8;

    printf("HID data: [");
    for(uint8_t i = 0; i < 44; i++) {
        if(result & ((uint64_t)1 << (43 - i))) {
            printf("1");
            data[out_idx] = 1;
            data[out_idx + 1] = 0;
            out_idx += 2;
        } else {
            printf("0");
            data[out_idx] = 0;
            data[out_idx + 1] = 1;
            out_idx += 2;
        }
    }
    printf("]\n\r");
}

void hid_emulation(uint8_t* data, GpioPin* pin) {
    taskENTER_CRITICAL();
    gpio_write(pin, false);

    for(uint8_t i = 0; i < 8; i++) {
        for(uint8_t j = 0; j < 96; j++) {
            uint8_t time = data[j] ? 40 : 32;

            for(uint8_t k = 0; k < 6; k++) {
                delay_us(time + i/2);
                gpio_write(pin, true);
                delay_us(time - i/2);
                gpio_write(pin, false);
            }
        }
    }

    gpio_write(pin, false);
    taskEXIT_CRITICAL();
}