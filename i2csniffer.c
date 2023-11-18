#include "i2csniffer.h"

void clear_sniffer_buffers(i2cSniffer* i2c_sniffer) {
    furi_assert(i2c_sniffer);
    for(uint8_t i = 0; i < MAX_RECORDS; i++) {
        for(uint8_t j = 0; j < MAX_MESSAGE_SIZE; j++) {
            i2c_sniffer->frames[i].ack[j] = false;
            i2c_sniffer->frames[i].data[j] = 0;
        }
        i2c_sniffer->frames[i].bit_index = 0;
        i2c_sniffer->frames[i].data_index = 0;
    }
    i2c_sniffer->saved = false;
    i2c_sniffer->frame_index = 0;
    i2c_sniffer->state = I2C_BUS_FREE;
    i2c_sniffer->first = true;
}

void start_interrupts(i2cSniffer* i2c_sniffer) {
    furi_assert(i2c_sniffer);
    furi_hal_gpio_init(pinSCL, GpioModeInterruptRise, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(pinSCL, SCLcallback, i2c_sniffer);

    // Add Rise and Fall Interrupt on SDA pin
    furi_hal_gpio_init(pinSDA, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedHigh);
    furi_hal_gpio_add_int_callback(pinSDA, SDAcallback, i2c_sniffer);
}

void stop_interrupts() {
    furi_hal_gpio_remove_int_callback(pinSCL);
    furi_hal_gpio_remove_int_callback(pinSDA);
    // Reset GPIO pins to default state
    furi_hal_gpio_init(pinSCL, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(pinSDA, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

// Called on Fallin/Rising SDA
// Used to monitor i2c bus state
void SDAcallback(void* _i2c_sniffer) {
    i2cSniffer* i2c_sniffer = _i2c_sniffer;
    // SCL is low maybe cclock strecching
    if(furi_hal_gpio_read(pinSCL) == false) {
        return;
    }
    // Check for stop condition: SDA rising while SCL is High
    if(i2c_sniffer->state == I2C_BUS_STARTED) {
        if(furi_hal_gpio_read(pinSDA) == true) {
            i2c_sniffer->state = I2C_BUS_FREE;
        }
    }
    // Check for start condition: SDA falling while SCL is high
    else if(furi_hal_gpio_read(pinSDA) == false) {
        i2c_sniffer->state = I2C_BUS_STARTED;
        if(i2c_sniffer->first) {
            i2c_sniffer->first = false;
            return;
        }
        i2c_sniffer->frame_index++;
        if(i2c_sniffer->frame_index >= MAX_RECORDS) {
            //save_sniffer_data(i2c_sniffer);
            clear_sniffer_buffers(i2c_sniffer);
        }
    }
    return;
}

// Called on Rising SCL
// Used to read bus datas
void SCLcallback(void* _i2c_sniffer) {
    i2cSniffer* i2c_sniffer = _i2c_sniffer;
    if(i2c_sniffer->state == I2C_BUS_FREE) {
        return;
    }
    uint8_t frame = i2c_sniffer->frame_index;
    uint8_t bit = i2c_sniffer->frames[frame].bit_index;
    uint8_t data_idx = i2c_sniffer->frames[frame].data_index;
    if(bit < 8) {
        i2c_sniffer->frames[frame].data[data_idx] <<= 1;
        i2c_sniffer->frames[frame].data[data_idx] |= (int)furi_hal_gpio_read(pinSDA);
        i2c_sniffer->frames[frame].bit_index++;
    } else {
        i2c_sniffer->frames[frame].ack[data_idx] = !furi_hal_gpio_read(pinSDA);
        i2c_sniffer->frames[frame].data_index++;
        i2c_sniffer->frames[frame].bit_index = 0;
    }
}

i2cSniffer* i2c_sniffer_alloc() {
    i2cSniffer* i2c_sniffer = malloc(sizeof(i2cSniffer));
    i2c_sniffer->started = false;
    i2c_sniffer->row_index = 0;
    i2c_sniffer->menu_index = 0;
    i2c_sniffer->must_save = false;
    clear_sniffer_buffers(i2c_sniffer);
    return i2c_sniffer;
}

void i2c_sniffer_free(i2cSniffer* i2c_sniffer) {
    furi_assert(i2c_sniffer);
    if(i2c_sniffer->started) {
        stop_interrupts();
    }
    free(i2c_sniffer);
}

bool save_sniffer_data(i2cSniffer* i2c_sniffer) {
    furi_assert(i2c_sniffer);
    size_t content_size = sizeof(i2cFrame) * (i2c_sniffer->frame_index + 1);
    char* content = (char*)malloc(content_size);
    snprintf(content, content_size, "%s\n", "i2c sniffer logs");
    size_t content_offset = strlen(content);

    for(size_t i = 0; i < i2c_sniffer->frame_index; i++) {
        content_offset = strlen(content);
        if((int)(i2c_sniffer->frames[i].data[0]) % 2 == 0) {
            snprintf(content + content_offset, content_size - content_offset, "%d: Write\n", i);
        } else {
            snprintf(content + content_offset, content_size - content_offset, "%d: Read\n", i);
        }
        content_offset = strlen(content);
        snprintf(
            content + content_offset,
            content_size - content_offset,
            "%d: Address: 0x%02x\n",
            i,
            (int)(i2c_sniffer->frames[i].data[0] >> 1));
        for(size_t j = 1; j <= i2c_sniffer->frames->data_index; j++) {
            content_offset = strlen(content);
            snprintf(
                content + content_offset,
                content_size - content_offset,
                "%d: Data: 0x%02x\n",
                i,
                (int)(i2c_sniffer->frames[i].data[j]));
        }
        content_offset = strlen(content);
        snprintf(content + content_offset, content_size - content_offset, "\n");
    }
    bool result = i2c_save_file(content);
    if(result) {
        i2c_sniffer->saved = true;
    }
    free(content);
    return result;
}