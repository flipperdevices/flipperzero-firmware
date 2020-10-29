extern "C" {
    #include "main.h"
    #include "cmsis_os.h"
    #include "gpio.h"
}

uint8_t onewire_reset(GpioPin pin);
uint8_t onewire_read_bit(GpioPin pin);
uint8_t onewire_read(GpioPin pin);
void onewire_read_bytes(GpioPin pin, uint8_t *buf, uint16_t count);
void onewire_write_bit(GpioPin pin, uint8_t v);
void onewire_write(GpioPin pin, uint8_t v);