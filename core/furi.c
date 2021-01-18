#include "furi.h"
#include <applications.h>

// for testing purpose
uint32_t exitcode = 0;

void set_exitcode(uint32_t _exitcode) {
    exitcode = _exitcode;
}

void furi_init() {
    furi_record_init();
    furi_stdglue_init();
}

int systemd() {
    furi_init();

    // FURI startup
    const size_t flipper_app_count = sizeof(FLIPPER_SERVICES) / sizeof(FLIPPER_SERVICES[0]);

    for(size_t i = 0; i < flipper_app_count; i++) {
        // furi_application_start(FLIPPER_SERVICES[i].app, FLIPPER_SERVICES[i].name, NULL);
        osThreadAttr_t* attr = furi_alloc(sizeof(osThreadAttr_t));
        attr->name = FLIPPER_SERVICES[i].name;
        attr->stack_size = 1024;
        osThreadNew(FLIPPER_SERVICES[i].app, NULL, attr);
    }

    while(1) {
        osThreadSuspend(osThreadGetId());
    }

    printf("\n=== Bye from Flipper Zero! ===\n\n");

    return (int)exitcode;
}