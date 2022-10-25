#include "i2csniffer.h"

void clearSnifferBuffers(i2csniffer* sniffer) {
    for(uint8_t i = 0; i < MAX_RECORDS; i++) {
        for(uint8_t j = 0; j < MAX_MESSAGE_SIZE; j++) {
            sniffer->frames[i].ack[j] = false;
            sniffer->frames[i].data[j] = 0;
        }
        sniffer->frames[i].bit_index = 0;
        sniffer->frames[i].data_index = 0;
    }
    sniffer->frame_index = 0;
    sniffer->state = I2C_BUS_FREE;
    sniffer->first = true;
}

void start_interrupts(i2csniffer* sniffer) {
    furi_hal_gpio_init(pinSCL, GpioModeInterruptRise, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(pinSCL, SCLcallback, sniffer);

    // Add Rise and Fall Interrupt on SDA pin
    furi_hal_gpio_init(pinSDA, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(pinSDA, SDAcallback, sniffer);
}

void stop_interrupts() {
    furi_hal_gpio_remove_int_callback(pinSCL);
    furi_hal_gpio_remove_int_callback(pinSDA);
}

// Called on Fallin/Rising SDA
// Used to monitor i2c bus state
void SDAcallback(void* _sniffer) {
    i2csniffer* sniffer = _sniffer;
    // SCL is low maybe cclock strecching
    if(furi_hal_gpio_read(pinSCL) == false) {
        return;
    }
    // Check for stop condition: SDA rising while SCL is High
    if(sniffer->state == I2C_BUS_STARTED) {
        if(furi_hal_gpio_read(pinSDA) == true) {
            sniffer->state = I2C_BUS_FREE;
        }
    }
    // Check for start condition: SDA falling while SCL is high
    else if(furi_hal_gpio_read(pinSDA) == false) {
        sniffer->state = I2C_BUS_STARTED;
        if(sniffer->first) {
            sniffer->first = false;
            return;
        }
        sniffer->frame_index++;
        if(sniffer->frame_index >= MAX_RECORDS) {
            clearSnifferBuffers(sniffer);
        }
    }
    return;
}

// Called on Rising SCL
// Used to read bus datas
void SCLcallback(void* _sniffer) {
    i2csniffer* sniffer = _sniffer;
    if(sniffer->state == I2C_BUS_FREE) {
        return;
    }
    uint8_t frame = sniffer->frame_index;
    uint8_t bit = sniffer->frames[frame].bit_index;
    uint8_t data_idx = sniffer->frames[frame].data_index;
    if(bit < 8) {
        sniffer->frames[frame].data[data_idx] <<= 1;
        sniffer->frames[frame].data[data_idx] |= (int)furi_hal_gpio_read(pinSDA);
        sniffer->frames[frame].bit_index++;
    } else {
        sniffer->frames[frame].ack[data_idx] = !furi_hal_gpio_read(pinSDA);
        sniffer->frames[frame].data_index++;
        sniffer->frames[frame].bit_index = 0;
    }
}
