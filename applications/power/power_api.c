#include "power.h"
#include <furi.h>
#include "furi-hal-power.h"
#include "furi-hal-boot.h"

void power_off(Power* power) {
    furi_assert(power);
    furi_hal_power_off();
}

void power_reboot(Power* power, PowerBootMode mode) {
    furi_assert(power);
    if(mode == PowerBootModeNormal) {
        furi_hal_boot_set_mode(FuriHalBootModeNormal);
    } else if(mode == PowerBootModeDfu) {
        furi_hal_boot_set_mode(FuriHalBootModeDFU);
    }
    furi_hal_power_reset();
}
