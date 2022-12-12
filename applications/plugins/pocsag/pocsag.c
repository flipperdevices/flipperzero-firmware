#include "pocsag.h"
#include "core/pocsag_hal.h"

#define TAG "POCSAG"

enum {
    PocsagSubmenuIndexReceive,
    PocsagSubmenuIndexStopReceive,
    PocsagSubmenuIndexTestSend,
};

#define FREQUENCY 433920000
//439987500


uint32_t pocsag_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}


void subghz_tx_rx_read_callback(void* context) {
    furi_assert(context);
    PocsagApp* app = context;
    uint8_t data[16];
    size_t cnt;

    FURI_LOG_I(TAG, "Callback");
    cnt = pocsag_tx_rx_worker_read(app->subghz_tx_rx, data, sizeof(data));
    FURI_LOG_I(TAG, "%d", cnt);
}


void pocsag_submenu_callback(void* context, uint32_t index) {
    uint8_t buf[] = {
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
    };
    furi_assert(context);
    PocsagApp* app = context;

    switch (index) {
        case PocsagSubmenuIndexReceive:
            if (pocsag_tx_rx_worker_is_running(app->subghz_tx_rx))
                FURI_LOG_I(TAG, "Receiver is already running...");
            else {
                FURI_LOG_I(TAG, "Start receiver...");
                furi_hal_power_suppress_charge_enter();
                pocsag_tx_rx_worker_start(app->subghz_tx_rx, FREQUENCY);
            }
            break;
        case PocsagSubmenuIndexStopReceive:
            FURI_LOG_I(TAG, "Stop receiver...");
            pocsag_tx_rx_worker_stop(app->subghz_tx_rx);
            furi_hal_power_suppress_charge_exit();
            break;
        case PocsagSubmenuIndexTestSend:
            if (pocsag_tx_rx_worker_is_running(app->subghz_tx_rx)) {
                pocsag_tx_rx_worker_write(app->subghz_tx_rx, buf, sizeof(buf));
            }
            break;
    }
}


PocsagApp* pocsag_app_alloc() {
    PocsagApp* app = malloc(sizeof(PocsagApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu, "Receive", PocsagSubmenuIndexReceive, pocsag_submenu_callback, app);
    submenu_add_item(app->submenu, "Stop receive", PocsagSubmenuIndexStopReceive, pocsag_submenu_callback, app);
    submenu_add_item(app->submenu, "Test send", PocsagSubmenuIndexTestSend, pocsag_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), pocsag_exit);
    view_dispatcher_add_view(
            app->view_dispatcher, PocsagViewSubmenu, submenu_get_view(app->submenu));

    app->view_id = PocsagViewSubmenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    app->subghz_tx_rx = pocsag_tx_rx_worker_alloc();
    pocsag_tx_rx_worker_set_callback_have_read(app->subghz_tx_rx, subghz_tx_rx_read_callback, app);
//    pocsag_hal_reset();

    return app;
}


void pocsag_app_free(PocsagApp* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, PocsagViewSubmenu);
    submenu_free(app->submenu);
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    if(pocsag_tx_rx_worker_is_running(app->subghz_tx_rx)) {
        pocsag_tx_rx_worker_stop(app->subghz_tx_rx);
        furi_hal_power_suppress_charge_exit();
    }
    pocsag_tx_rx_worker_free(app->subghz_tx_rx);
    free(app);
}


int32_t pocsag(void* p) {
    UNUSED(p);

    PocsagApp* app = pocsag_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    pocsag_app_free(app);
    return 0;
}
