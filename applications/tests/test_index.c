#include <stdio.h>
#include <furi.h>
#include <api-hal.h>
#include "minunit_vars.h"

int run_minunit();
int run_minunit_test_irda_decoder();

int32_t flipper_test_app(void* p) {
    api_hal_light_set(LightRed, 0x00);
    api_hal_light_set(LightGreen, 0x00);
    api_hal_light_set(LightBlue, 0xFF);

    uint32_t exit_code = run_minunit();
    uint32_t exit_code_irda_decoder = run_minunit_test_irda_decoder();

    if((exit_code == 0) && (exit_code_irda_decoder == 0)) {
        // test passed
        api_hal_light_set(LightRed, 0x00);
        api_hal_light_set(LightGreen, 0xFF);
        api_hal_light_set(LightBlue, 0x00);
    } else {
        // test failed
        api_hal_light_set(LightRed, 0xFF);
        api_hal_light_set(LightGreen, 0x00);
        api_hal_light_set(LightBlue, 0x00);
    }

    /* power_charging_indication_handler() breaks 1 sec light on, but at least it lights a little */
    delay(1000);

    return 0;
}
