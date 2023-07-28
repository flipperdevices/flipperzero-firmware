// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "../ublox_i.h"
#include "../ublox_worker_i.h"

#define TAG "ublox_scene_data_display"

const NotificationSequence sequence_new_reading = {
    //&message_vibro_on,
    &message_green_255,
    &message_delay_100,
    &message_green_0,
    //&message_vibro_off,
    NULL,
};

void ublox_scene_data_display_worker_callback(UbloxWorkerEvent event, void* context) {
    Ublox* ublox = context;

    view_dispatcher_send_custom_event(ublox->view_dispatcher, event);
}

void ublox_scene_data_display_view_callback(void* context, InputKey key) {
    Ublox* ublox = context;

    // just reuse generic events
    if(key == InputKeyLeft) {
        view_dispatcher_send_custom_event(ublox->view_dispatcher, GuiButtonTypeLeft);
    } else if(key == InputKeyOk) {
        view_dispatcher_send_custom_event(ublox->view_dispatcher, GuiButtonTypeCenter);
    }
}

static void timer_callback(void* context) {
    Ublox* ublox = context;
    FURI_LOG_I(TAG, "mem free before timer callback: %u", memmgr_get_free_heap());
    // every time, try to set the view back to a GPS-found mode

    // TODO: maybe different states for each message, to fix the leak?
    ublox_worker_start(
        ublox->worker, UbloxWorkerStateRead, ublox_scene_data_display_worker_callback, ublox);
    FURI_LOG_I(TAG, "mem free after timer callback: %u", memmgr_get_free_heap());
}

void ublox_scene_data_display_on_enter(void* context) {
    Ublox* ublox = context;

    data_display_set_callback(ublox->data_display, ublox_scene_data_display_view_callback, ublox);
    if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeHandheld) {
        data_display_set_state(ublox->data_display, DataDisplayHandheldMode);
    } else if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeCar) {
        data_display_set_state(ublox->data_display, DataDisplayCarMode);
    }

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewDataDisplay);

    ublox->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, ublox);

    // convert from seconds to milliseconds
    furi_timer_start(
        ublox->timer, furi_ms_to_ticks((ublox->data_display_state).refresh_rate * 1000));

    ublox_worker_start(
        ublox->worker, UbloxWorkerStateRead, ublox_scene_data_display_worker_callback, ublox);
}

bool ublox_scene_data_display_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    bool consumed = false;
    //FURI_LOG_I(TAG, "mem free before event branch: %u", memmgr_get_free_heap());
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            FURI_LOG_I(TAG, "left button pressed");
            scene_manager_next_scene(ublox->scene_manager, UbloxSceneDataDisplayConfig);
            consumed = true;

        } else if(event.event == GuiButtonTypeCenter) {
            furi_timer_stop(ublox->timer);
            // MUST stop the worker first! (idk why though)
            ublox_worker_stop(ublox->worker);
            FURI_LOG_I(TAG, "reset odometer");
            ublox_worker_start(
                ublox->worker,
                UbloxWorkerStateResetOdometer,
                ublox_scene_data_display_worker_callback,
                ublox);
            furi_timer_start(
                ublox->timer, furi_ms_to_ticks((ublox->data_display_state).refresh_rate * 1000));

        } else if(event.event == UbloxWorkerEventDataReady) {
            if((ublox->data_display_state).notify_mode == UbloxDataDisplayNotifyOn) {
                notification_message(ublox->notifications, &sequence_new_reading);
            }

            // Not setting the NAV messages in the data display seems to help...?
            // (so many things have "seemed to help" that it's immensely confusing).

            // disabling refresh in the with_view_model() call in the update function kinda helps
            data_display_set_nav_messages(ublox->data_display, ublox->nav_pvt, ublox->nav_odo);

            // There used to be a "set view_mode" if/else here. I don't
            // think it was necessary but I do think it was contributing to
            // the memory leak. With it removed, we get little to no leakage
            // (and the leakage "automatically" fixes itself: some memory
            // will be incorrectly allocated but then fixed).

            // The upshot is that we used to refresh the data display scene
            // 3 times in this callback. I suspect that the view handling
            // code in the OS is good but not perfect, and holds on to
            // memory for longer than it needs to. Reducing the number of
            // total updates has helped.

        } else if(event.event == UbloxWorkerEventFailed) {
            FURI_LOG_I(TAG, "UbloxWorkerEventFailed");
            data_display_set_state(ublox->data_display, DataDisplayGPSNotFound);
        }
    }
    //FURI_LOG_I(TAG, "mem free after event branch: %u", memmgr_get_free_heap());
    return consumed;
}

void ublox_scene_data_display_on_exit(void* context) {
    Ublox* ublox = context;

    furi_timer_stop(ublox->timer);
    furi_timer_free(ublox->timer);

    ublox_worker_stop(ublox->worker);

    data_display_reset(ublox->data_display);
    // Use any existing data
    data_display_set_nav_messages(ublox->data_display, ublox->nav_pvt, ublox->nav_odo);
    FURI_LOG_I(TAG, "leaving data display scene");
}
