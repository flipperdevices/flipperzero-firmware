#include "arduino.h"

const char* getPinName(const GpioPin* pin) {
    if(pin == &gpio_ext_pc3) return "out";
    if(pin == &gpio_ext_pb3) return "notOE";
    if(pin == &gpio_ext_pa4) return "Ain";
    if(pin == &gpio_ext_pb2) return "probe";
    return "unknown";
}

const char* F(const char* i) {
    return i;
}

void delay(int ms) {
    furi_delay_ms(ms);
}

void delayMicroseconds(int us) {
    furi_delay_us(us);
}

uint32_t micros() {
    return DWT->CYCCNT / furi_hal_cortex_instructions_per_microsecond();
}

uint32_t millis() {
    return micros() / 1000;
}
