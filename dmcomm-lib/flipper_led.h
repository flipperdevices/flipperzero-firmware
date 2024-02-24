#pragma once

#include "DMComm.h"
#include "dmcomm-lib/dmcomm_led.h"
#include "app_state.h"

/*

*/

namespace DMComm {

class FlipperIndicator : public Indicator {
public:
    FlipperIndicator(App* app);
    void begin();
    void end();
    void new_digirom(BaseDigiROM * digirom);
    void new_digirom_error();
    void executed_digirom(BaseDigiROM * digirom);

private:
    App* app;
};

}