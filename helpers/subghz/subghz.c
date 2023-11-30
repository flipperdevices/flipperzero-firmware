/* Reduced variant of the Flipper Zero SubGhz Class */

#include "subghz_i.h"

static SubGhz* subghz_alloc() {
    SubGhz* subghz = malloc(sizeof(SubGhz));
    
    subghz->file_path = furi_string_alloc();

    subghz->txrx = subghz_txrx_alloc();

    return subghz;
}

static void subghz_free(SubGhz* subghz) {
    //TxRx
    subghz_txrx_free(subghz->txrx);

    // Furi strings
    furi_string_free(subghz->file_path);

    // The rest
    free(subghz);
}

void subghz_send(void* context) {
    UNUSED(context);
    SubGhz* subghz = subghz_alloc();

    

    subghz_free(subghz);
}