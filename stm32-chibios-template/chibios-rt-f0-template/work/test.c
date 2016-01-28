#include "ch.h"
#include "hal.h"
#include "test.h"
/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palSetPad(GPIOC, GPIOC_LED4);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOC, GPIOC_LED4);
    chThdSleepMilliseconds(500);
  }
}

void testInit(void)
{
    /*
     * Creates the example thread.
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
}
