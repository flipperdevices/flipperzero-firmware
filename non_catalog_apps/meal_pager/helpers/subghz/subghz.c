/* Reduced variant of the Flipper Zero SubGhz Class */

#include "subghz_i.h"
#include "../../helpers/meal_pager_custom_event.h"
#include "../../helpers/meal_pager_led.h"
//#include "../meal_pager_storage.h"

SubGhz* subghz_alloc() {
    SubGhz* subghz = malloc(sizeof(SubGhz));

    subghz->file_path = furi_string_alloc();

    subghz->txrx = subghz_txrx_alloc();

    return subghz;
}

void subghz_free(SubGhz* subghz) {
    //TxRx
    subghz_txrx_free(subghz->txrx);

    // Furi strings
    furi_string_free(subghz->file_path);

    // The rest
    free(subghz);
}

void subghz_scene_transmit_callback_end_tx(void* context) {
    furi_assert(context);
    //UNUSED(context);
    FURI_LOG_D(TAG, "callback end");
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, Meal_PagerCustomEventViewTransmitterSendStop);
}

void subghz_send(void* context) {
    //UNUSED(context);
    Meal_Pager* app = context;
    //SubGhz* subghz = subghz_alloc();

    FURI_LOG_D(TAG, "loading protocol from file");
    subghz_load_protocol_from_file(app->subghz);

    /*Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    if(!flipper_format_file_open_existing(ff, MEAL_PAGER_TMP_FILE)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error reading Temp File %s", MEAL_PAGER_TMP_FILE);
        furi_record_close(RECORD_STORAGE);
        return;
    }*/

    //subghz_txrx_tx_start(subghz->txrx, ff);

    FURI_LOG_D(TAG, "Starting Transmission");
    subghz_txrx_tx_start(
        app->subghz->txrx,
        subghz_txrx_get_fff_data(app->subghz->txrx)); //Seems like it must be done this way

    FURI_LOG_D(TAG, "setting sugbhz raw file encoder worker callback");
    subghz_txrx_set_raw_file_encoder_worker_callback_end(
        app->subghz->txrx, subghz_scene_transmit_callback_end_tx, app);
    app->state_notifications = SubGhzNotificationStateTx;

    /*flipper_format_rewind(ff);
    flipper_format_file_close(ff);
    flipper_format_free(ff);

    furi_record_close(RECORD_STORAGE);*/

    //subghz_free(subghz);
    FURI_LOG_D(TAG, "Finished Transmitting");
    //meal_pager_blink_stop(app);
}