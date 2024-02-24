// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_LED_H_
#define DMCOMM_LED_H_

#include "DMComm.h"

namespace DMComm {

class Indicator {
public:
    virtual ~Indicator() {
    }
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void new_digirom(BaseDigiROM* digirom) = 0;
    virtual void new_digirom_error() = 0;
    virtual void executed_digirom(BaseDigiROM* digirom) = 0;
};

#ifdef NOT_FLIPPER
class LED_Indicator : public Indicator {
public:
    LED_Indicator(uint8_t pin);
    void begin();
    void end();
    void new_digirom(BaseDigiROM* digirom);
    void new_digirom_error();
    void executed_digirom(BaseDigiROM* digirom);

private:
    uint8_t pin_;
};
#endif

} // namespace DMComm

#endif // DMCOMM_LED_H_
