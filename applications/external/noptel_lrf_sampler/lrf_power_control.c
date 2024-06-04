/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Power control
***/

/*** Includes ***/
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include "common.h"

/*** Routines ***/

/** Turn the LRF on or off
    Control the LRF using the C1 pin **/
void power_lrf(bool on) {
#ifdef USE_5V_PIN
    uint8_t otg_on_attempt = 0;
#endif

    /* Should we turn the LRF on? */
    if(on) {
        /* Set pin C1 to output with push-pull resistors */
        furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);

        /* Set the pin high so it outputs 3.3V */
        furi_hal_gpio_write(&gpio_ext_pc1, true);

#ifdef USE_5V_PIN
        /* Set the +5V pin high */
        while(otg_on_attempt < 5) {
            if(furi_hal_power_is_otg_enabled()) break;
            furi_hal_power_enable_otg();
            furi_delay_ms(10);
            otg_on_attempt++;
        }
#endif

        /* Wait a bit to let the LRF boot up and be ready to accept commands */
        furi_delay_ms(300);
    }

    else {
#ifdef USE_5V_PIN
        /* Set the +5V pin low */
        furi_hal_power_disable_otg();
#endif

        /* Set the C1 pin low */
        furi_hal_gpio_write(&gpio_ext_pc1, false);

        /* Reset the pin to its default state */
        furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeAnalog);
    }
}
