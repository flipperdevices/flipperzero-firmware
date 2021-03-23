#include "subghz.h"

#include <furi.h>

typedef struct {
    bool is_working;
} SubGhz;

SubGhz* subghz_alloc() {
    SubGhz* subghz = furi_alloc(sizeof(SubGhz));
    subghz->is_working = true;

    return subghz;
}

void subghz_free(SubGhz* subghz) {
    furi_assert(subghz);
    free(subghz);
}

int32_t subghz_app(void* context) {
    SubGhz* subghz = subghz_alloc();

    // while(subghz->is_working) {
    //     osDelay(1024);
    // }

    subghz_free(subghz);

    return 0;
}