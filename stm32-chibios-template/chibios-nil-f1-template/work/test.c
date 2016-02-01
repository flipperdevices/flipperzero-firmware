#include "nil.h"
#include "hal.h"
#include "test.h"
/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
THD_WORKING_AREA(waThread1, 128);
THD_FUNCTION(Thread1, arg) {

  (void)arg;
  while (true) {
    palSetPad(GPIOC, GPIOC_LED4);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOC, GPIOC_LED4);
    chThdSleepMilliseconds(500);
  }
}


