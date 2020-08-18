#include "flipper.h"
#include <stdio.h>

extern "C" {
    #include "startup.h"
    #include "furi.h"
    #include "debug.h"
}

extern "C" void app() {
    // FURI startup
    for(size_t i = 0; i < sizeof(FLIPPER_STARTUP)/sizeof(FLIPPER_STARTUP[0]); i++) {
        furiac_start(FLIPPER_STARTUP[i].app, FLIPPER_STARTUP[i].name, NULL);
    }

    /*
    FILE* debug_uart = get_debug();

    fprintf(debug_uart, "hello Flipper!\n");

    GpioPin red_led = {LED_RED_GPIO_Port, LED_RED_Pin};

    app_gpio_init(red_led, GpioModeOutput);

    
    while(1) {
        delay(100);
        app_gpio_write(red_led, true);
        delay(100);
        app_gpio_write(red_led, false);
    }
    */

    while(1) {
        printf("furi main thread\n");
        delay(2000);
    }
}