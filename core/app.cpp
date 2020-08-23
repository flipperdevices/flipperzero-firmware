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

    while(1) {
        delay(10000); // TODO add deferred event queue
    }
}