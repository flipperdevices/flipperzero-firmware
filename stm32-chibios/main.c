#include "hal.h"
#include "nil.h"

THD_WORKING_AREA(waBlinky, 128);
THD_FUNCTION(blinky, arg)
{
    (void)arg;
    for(;;)
    {
        palSetPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);

        palClearPad(GPIOH, GPIOH_LED1);
        chThdSleepMilliseconds(500);
    }
}


THD_WORKING_AREA(waHello, 128);
THD_FUNCTION(hello, arg)
{
    (void)arg;

    palSetPadMode(GPIOC, GPIOC_USART3_TX, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOC, GPIOC_USART3_RX, PAL_MODE_ALTERNATE(7));

    sdStart(&SD3, NULL);

    for (;;)
    {
        chnWrite((BaseChannel*)&SD3, "Hello, World!\r\n", sizeof("Hello, World!\r\n"));
        chThdSleepMilliseconds(1000);
    }
}

THD_TABLE_BEGIN
  THD_TABLE_ENTRY(waBlinky, "blinky", blinky, NULL)
  THD_TABLE_ENTRY(waHello, "hello", hello, NULL)
THD_TABLE_END

int main(void) 
{
    halInit();
    chSysInit();

    for(;;)
    {
    }
}
