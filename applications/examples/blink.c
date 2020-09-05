#include "flipper.h"

void application_blink(void* p) {
    // create pin
    GpioPin led = LED_RED;
    
    // configure pin
    pinMode(led, GpioModeOpenDrain);

    while(1) {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
        delay(500);
    }
}
