#include "ch.h"
#include "hal.h"

static WORKING_AREA(blinkyThreadArea, 128);

static msg_t blinky(void *arg)
{
    (void)arg;
    chRegSetThreadName("blinky");
    for(;;)
    {
        palSetPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);

        palClearPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);
    }
}


static msg_t hello(void *arg)
{
    (void)arg;
    chRegSetThreadName("hello");
    while (TRUE)
    {
        palSetPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);

        palClearPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);
    }
}

int main(void) 
{
    halInit();
    chSysInit();

    sdStart(&SD3, NULL);
    palSetPadMode(GPIOC, GPIOC_USART3_TX, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOC, GPIOC_USART3_RX, PAL_MODE_ALTERNATE(7));

    chThdCreateStatic(blinkyThreadArea, sizeof(blinkyThreadArea), NORMALPRIO, blinky, NULL);
    for(;;)
    {
        chSequentialStreamWrite((BaseSequentialStream*)&SD3, "Hello, World!\r\n", sizeof("Hello, World!\r\n"));
        chThdSleepMilliseconds(1000);
    }
}
