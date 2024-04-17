/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
 *
 * Power control
***/

/*** Includes ***/
#include <furi_hal.h>
#include <furi_hal_gpio.h>



/*** Routines ***/

/** Turn the LRF on or off
    Control the LRF using the C1 pin **/
void power_lrf(bool on) {

  /* Should we turn the LRF on? */
  if(on) {

    /* Set pin C1 to output with push-pull resistors */
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);

    /* Set the pin to true so it outputs 3.3V */
    furi_hal_gpio_write(&gpio_ext_pc1, true);

    /* Wait a bit to let the LRF boot up and be ready to accept commands */
    furi_delay_ms(300);
  }

  else {

    /* Set the C1 pin to false so it outputs 0V */
    furi_hal_gpio_write(&gpio_ext_pc1, false);

    /* Reset the pin to its default state */
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeAnalog);
  }
}
