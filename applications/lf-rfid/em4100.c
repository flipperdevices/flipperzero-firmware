#include "flipper_v2.h"

void prepare_data(uint32_t ID, uint32_t VENDOR, uint8_t* data){
    uint8_t value[10];
    
    value[0] = (VENDOR>>4) & 0XF;
    value[1] = VENDOR & 0XF;
    for (int i=1; i<8; i++){
        value[i+2] = (ID>>(28-i*4)) &0xF;
    }

    for (uint8_t i = 0; i < 9; i++) data[i]=1; // header
    for (uint8_t i = 0; i < 10; i++) {         // data
        for (uint8_t j = 0; j < 4; j++) {
            data[9 + i*5 +j] = value[i] >> (3-j) & 1;
        }

        data[9 + i*5 + 4] = ( data[9 + i*5 + 0]
                            + data[9 + i*5 + 1]
                            + data[9 + i*5 + 2]
                            + data[9 + i*5 + 3]) % 2;
    }
    
    for (uint8_t i = 0; i < 4; i++) { //checksum
        int checksum=0;
        for (uint8_t j = 0; j < 10; j++) {
            checksum += data[9 + i + j*5];
        }
        data[i+59] = checksum%2;
    }

    data[63] = 0; //footer
}

void em4100_emulation(uint8_t* data, GpioPin* pin) {
    for (uint8_t i = 0; i < 15; i++){
        for (uint8_t j = 0; j < 64; j++){
            gpio_write(pin, !data[j]);
            delay_us(200);
            gpio_write(pin, data[j]);
            delay_us(200);
        }
    }
}