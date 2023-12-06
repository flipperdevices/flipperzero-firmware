/* Reduced variant of the Flipper Zero SubGhz Class */

#include "subghz_i.h"
#include "../meal_pager_storage.h"

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

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    if(!flipper_format_file_open_existing(ff, MEAL_PAGER_TMP_FILE)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error creating new file %s", MEAL_PAGER_TMP_FILE);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    subghz_txrx_tx_start(subghz->txrx, ff);

    flipper_format_rewind(ff);
    flipper_format_file_close(ff);
    flipper_format_free(ff);

    furi_record_close(RECORD_STORAGE);


    subghz_free(subghz);
}