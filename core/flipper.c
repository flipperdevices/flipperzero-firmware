#include "flipper.h"
#include <applications.h>

void flipper_init() {
    printf("[flipper] Build date:" BUILD_DATE ". "
           "Git Commit:" GIT_COMMIT ". "
           "Git Branch:" GIT_BRANCH ". "
           "Commit Number:" GIT_BRANCH_NUM "\r\n");

    printf("[flipper] starting services\r\n");
    for(size_t i = 0; i < FLIPPER_SERVICES_COUNT; i++) {
        printf("[flipper] starting service %s\r\n", FLIPPER_SERVICES[i].name);
        osThreadAttr_t* attr = furi_alloc(sizeof(osThreadAttr_t));
        attr->name = FLIPPER_SERVICES[i].name;
        attr->stack_size = FLIPPER_SERVICES[i].stack_size;
        osThreadNew(FLIPPER_SERVICES[i].app, NULL, attr);
    }
    printf("[flipper] services startup complete\r\n");
}