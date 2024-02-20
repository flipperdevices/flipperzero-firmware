/* This file is part of the DMComm project by BladeSabre. License: MIT. */

#include "DMComm.h"

namespace DMComm {

#ifdef NOT_FLIPPER
void pinModeMaybe(uint8_t pin, uint8_t mode) {
    if (pin != DMCOMM_NO_PIN) {
        pinMode(pin, mode);
    }
}

void digitalWriteMaybe(uint8_t pin, uint8_t val) {
    if (pin != DMCOMM_NO_PIN) {
        digitalWrite(pin, val);
    }
}
#else
void pinModeMaybe(uint8_t pin, uint8_t mode) {
    UNUSED(mode);
    if (pin != DMCOMM_NO_PIN) {
        //pinMode(pin, mode);
    }
}

void digitalWriteMaybe(uint8_t pin, uint8_t val) {
    UNUSED(val);
    if (pin != DMCOMM_NO_PIN) {
        //digitalWrite(pin, val);
    }
}
#endif
} /* namespace DMComm */
