#include "../seader_i.h"
#include "../seader_bridge.h"
#define TAG "SeaderSceneUart"

typedef struct {
    SeaderUartConfig cfg;
    SeaderUartState state;
} SceneUartBridge;

static SceneUartBridge* scene_uart;

void seader_uart_worker_callback(SeaderWorkerEvent event, void* context) {
    UNUSED(event);
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventWorkerExit);
}

void seader_scene_uart_callback(SeaderCustomEvent event, void* context) {
    furi_assert(context);
    Seader* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void seader_scene_uart_on_enter(void* context) {
    Seader* app = context;
    uint32_t prev_state = scene_manager_get_scene_state(app->scene_manager, SeaderViewUart);
    if(prev_state == 0) {
        scene_uart = malloc(sizeof(SceneUartBridge));
        scene_uart->cfg.uart_ch = 0;
        scene_uart->cfg.flow_pins = 0;
        scene_uart->cfg.baudrate_mode = 0;
        scene_uart->cfg.baudrate = 0;
    }

    seader_uart_get_config(app->uart, &scene_uart->cfg);
    seader_uart_get_state(app->uart, &scene_uart->state);

    seader_uart_view_set_callback(app->seader_uart_view, seader_scene_uart_callback, app);
    scene_manager_set_scene_state(app->scene_manager, SeaderSceneUart, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, SeaderViewUart);
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);

    Seader* seader = app;

    if(seader->credential->type == SeaderCredentialTypePicopass) {
        seader_worker_start(
            seader->worker,
            SeaderWorkerStateReadPicopass,
            seader->uart,
            seader->credential,
            seader_uart_worker_callback,
            seader);
    } else if(seader->credential->type == SeaderCredentialType14A) {
        seader_worker_start(
            seader->worker,
            SeaderWorkerStateRead14a,
            seader->uart,
            seader->credential,
            seader_uart_worker_callback,
            seader);
    }
}

bool seader_scene_uart_on_event(void* context, SceneManagerEvent event) {
    Seader* app = context;
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventWorkerExit) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneReadCardSuccess);
            consumed = true;
        } else if(event.type == SceneManagerEventTypeTick) {
            scene_manager_set_scene_state(app->scene_manager, SeaderSceneUart, 1);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        uint32_t tx_cnt_last = scene_uart->state.tx_cnt;
        uint32_t rx_cnt_last = scene_uart->state.rx_cnt;
        seader_uart_get_state(app->uart, &scene_uart->state);
        if(seader->credential->type == SeaderCredentialTypePicopass) {
            scene_uart->state.protocol = FrameProtocol_iclass;
        } else if(seader->credential->type == SeaderCredentialType14A) {
            scene_uart->state.protocol = FrameProtocol_nfc;
        }
        seader_uart_view_update_state(app->seader_uart_view, &scene_uart->cfg, &scene_uart->state);
        if(tx_cnt_last != scene_uart->state.tx_cnt) {
            notification_message(app->notifications, &sequence_blink_blue_10);
        }
        if(rx_cnt_last != scene_uart->state.rx_cnt) {
            notification_message(app->notifications, &sequence_blink_green_10);
        }
    }
    return consumed;
}

void seader_scene_uart_on_exit(void* context) {
    Seader* app = context;
    seader_worker_stop(app->worker);
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
}
