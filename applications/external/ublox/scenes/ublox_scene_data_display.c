#include "../ublox_i.h"
#include "../ublox_worker_i.h"

#define TAG "ublox_scene_data_display"

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
    } else if(key == InputKeyRight) {
        view_dispatcher_send_custom_event(ublox->view_dispatcher, GuiButtonTypeRight);
    }
}

void ublox_scene_data_display_on_enter(void* context) {
    Ublox* ublox = context;

    // Use any existing data
    data_display_set_nav_messages(ublox->data_display, ublox->nav_pvt, ublox->nav_odo);

    data_display_set_callback(ublox->data_display, ublox_scene_data_display_view_callback, ublox);

    if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeHandheld) {
        data_display_set_state(ublox->data_display, DataDisplayHandheldMode);
    } else if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeCar) {
        data_display_set_state(ublox->data_display, DataDisplayCarMode);
    }

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewDataDisplay);

    ublox_worker_start(
        ublox->worker, UbloxWorkerStateRead, ublox_scene_data_display_worker_callback, ublox);
}

bool ublox_scene_data_display_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            ublox_worker_stop(ublox->worker);
            scene_manager_next_scene(ublox->scene_manager, UbloxSceneDataDisplayConfig);
            consumed = true;

        } else if(event.event == GuiButtonTypeRight) {
            // TODO: only allow if GPS is detected?
            if(data_display_get_state(ublox->data_display) != DataDisplayGPSNotFound) {
                FURI_LOG_I(TAG, "right button");
                if(ublox->log_state == UbloxLogStateNone) {
                    // start logging
                    ublox_worker_stop(ublox->worker);
                    scene_manager_next_scene(ublox->scene_manager, UbloxSceneEnterFileName);
                    consumed = true;
                } else if(ublox->log_state == UbloxLogStateLogging) {
                    FURI_LOG_I(TAG, "stop logging from scene");
                    ublox->log_state = UbloxLogStateStopLogging;
                }
            }

        } else if(event.event == UbloxWorkerEventDataReady) {
            if((ublox->data_display_state).notify_mode == UbloxDataDisplayNotifyOn) {
                notification_message(ublox->notifications, &sequence_new_reading);
            }

            if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeHandheld) {
                data_display_set_state(ublox->data_display, DataDisplayHandheldMode);
            } else if((ublox->data_display_state).view_mode == UbloxDataDisplayViewModeCar) {
                data_display_set_state(ublox->data_display, DataDisplayCarMode);
            }

            data_display_set_nav_messages(ublox->data_display, ublox->nav_pvt, ublox->nav_odo);

        } else if(event.event == UbloxWorkerEventLogStateChanged) {
            data_display_set_log_state(ublox->data_display, ublox->log_state);

        } else if(event.event == UbloxWorkerEventFailed) {
            FURI_LOG_I(TAG, "UbloxWorkerEventFailed");
            data_display_set_state(ublox->data_display, DataDisplayGPSNotFound);
        }
    }
    return consumed;
}

void ublox_scene_data_display_on_exit(void* context) {
    Ublox* ublox = context;

    /*if(ublox->log_state == UbloxLogStateLogging) {
	FURI_LOG_I(TAG, "stop logging on exit");
	ublox->log_state = UbloxLogStateStopLogging;
	//while (ublox->log_state != UbloxLogStateNone);
	//furi_delay_ms(500);
	}*/

    ublox_worker_stop(ublox->worker);

    data_display_reset(ublox->data_display);
}
