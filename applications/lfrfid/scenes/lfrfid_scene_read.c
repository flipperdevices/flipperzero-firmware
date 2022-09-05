#include "../lfrfid_i.h"
#include <dolphin/dolphin.h>

static void
    lfrfid_read_callback(LFRFIDWorkerReadResult result, ProtocolId protocol, void* context) {
    LfRfid* app = context;
    uint32_t event = 0;

    if(result == LFRFIDWorkerReadSenseStart) {
        event = LfRfidEventReadSenseStart;
    } else if(result == LFRFIDWorkerReadSenseEnd) {
        event = LfRfidEventReadSenseEnd;
    } else if(result == LFRFIDWorkerReadSenseCardStart) {
        event = LfRfidEventReadSenseCardStart;
    } else if(result == LFRFIDWorkerReadSenseCardEnd) {
        event = LfRfidEventReadSenseCardEnd;
    } else if(result == LFRFIDWorkerReadDone) {
        event = LfRfidEventReadDone;
        app->protocol_id_next = protocol;
    } else if(result == LFRFIDWorkerReadStartASK) {
        event = LfRfidEventReadStartASK;
    } else if(result == LFRFIDWorkerReadStartPSK) {
        event = LfRfidEventReadStartPSK;
    } else {
        return;
    }

    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void lfrfid_scene_read_on_enter(void* context) {
    LfRfid* app = context;
    Popup* popup = app->popup;

    DOLPHIN_DEED(DolphinDeedRfidRead);
    if(app->read_type == LFRFIDWorkerReadTypePSKOnly) {
        popup_set_header(popup, "Reading\nLF RFID\nPSK", 89, 30, AlignCenter, AlignTop);
    } else {
        popup_set_header(popup, "Reading\nLF RFID\nASK", 89, 30, AlignCenter, AlignTop);
    }

    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    lfrfid_worker_start_thread(app->lfworker);
    lfrfid_worker_read_start(app->lfworker, app->read_type, lfrfid_read_callback, app);

    notification_message(app->notifications, &sequence_blink_start_cyan);

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewPopup);
}

bool lfrfid_scene_read_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    Popup* popup = app->popup;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == LfRfidEventReadSenseStart) {
            notification_message(app->notifications, &sequence_blink_stop);
            notification_message(app->notifications, &sequence_blink_start_yellow);
            consumed = true;
        } else if(event.event == LfRfidEventReadSenseCardStart) {
            notification_message(app->notifications, &sequence_blink_stop);
            notification_message(app->notifications, &sequence_blink_start_green);
            consumed = true;
        } else if(
            (event.event == LfRfidEventReadSenseEnd) ||
            (event.event == LfRfidEventReadSenseCardEnd)) {
            notification_message(app->notifications, &sequence_blink_stop);
            notification_message(app->notifications, &sequence_blink_start_cyan);
            consumed = true;
        } else if(event.event == LfRfidEventReadDone) {
            app->protocol_id = app->protocol_id_next;
            DOLPHIN_DEED(DolphinDeedRfidReadSuccess);
            notification_message(app->notifications, &sequence_success);
            string_reset(app->file_name);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneReadSuccess);
            consumed = true;
        } else if(event.event == LfRfidEventReadStartPSK) {
            popup_set_header(popup, "Reading\nLF RFID\nPSK", 89, 30, AlignCenter, AlignTop);
            consumed = true;
        } else if(event.event == LfRfidEventReadStartASK) {
            popup_set_header(popup, "Reading\nLF RFID\nASK", 89, 30, AlignCenter, AlignTop);
            consumed = true;
        }
    }

    return consumed;
}

void lfrfid_scene_read_on_exit(void* context) {
    LfRfid* app = context;
    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);
    lfrfid_worker_stop(app->lfworker);
    lfrfid_worker_stop_thread(app->lfworker);
}
