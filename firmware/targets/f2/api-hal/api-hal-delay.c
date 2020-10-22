#include "api-hal-delay.h"

void delay_us(uint32_t time) {
    time *= 11.8;

    while(time--) {
    }
}