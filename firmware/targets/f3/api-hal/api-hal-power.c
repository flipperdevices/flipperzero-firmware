#include <api-hal-power.h>
#include <bq27220.h>

uint8_t api_hal_power_get_pct() {
    return bq27220_get_state_of_charge();
}

bool api_hal_power_is_charging() {
    return bq27220_get_current() > 0;
}
