// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

#ifdef NOT_FLIPPER
LED_Indicator::LED_Indicator(uint8_t pin)
    : pin_(pin) {
}

void LED_Indicator::begin() {
    pinMode(pin_, OUTPUT);
}

void LED_Indicator::end() {
}

void LED_Indicator::new_digirom(BaseDigiROM* digirom) {
    UNUSED(digirom);
    for(uint8_t i = 0; i < 3; i++) {
        digitalWrite(pin_, HIGH);
        delay(50);
        digitalWrite(pin_, LOW);
        delay(50);
    }
}

void LED_Indicator::new_digirom_error() {
}

void LED_Indicator::executed_digirom(BaseDigiROM* digirom) {
    digitalWrite(pin_, HIGH);
    if(digirom->somethingReceived()) {
        //TODO errors too?
        delay(200);
    } else {
        delay(50);
    }
    digitalWrite(pin_, LOW);
}
#endif

} // namespace DMComm
