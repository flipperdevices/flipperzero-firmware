#include "flipper.h"

void application_uart_write(void* p) {
    // create pin
    GpioPin led = {.pin = GPIO_PIN_8, .port = GPIOA};
    
    // configure pin
    pinMode(led, GpioModeOpenDrain);

    while(1) {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
        delay(500);
    }
}