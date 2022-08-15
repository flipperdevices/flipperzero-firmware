#include "lfrfid_app_scene_read.h"
#include <dolphin/dolphin.h>

static void lfrfid_read_callback(LFRFIDWorkerReadResult result, ProtocolId protocol, void* ctx) {
    LfRfidApp* app = static_cast<LfRfidApp*>(ctx);
    LfRfidApp::Event event;

    switch(result) {
    case LFRFIDWorkerReadSenseStart:
        event.type = LfRfidApp::EventType::ReadEventSenseStart;
        break;
    case LFRFIDWorkerReadSenseEnd:
        event.type = LfRfidApp::EventType::ReadEventSenseEnd;
        break;
    case LFRFIDWorkerReadSenseCardStart:
        event.type = LfRfidApp::EventType::ReadEventSenseCardStart;
        break;
    case LFRFIDWorkerReadSenseCardEnd:
        event.type = LfRfidApp::EventType::ReadEventSenseCardEnd;
        break;
    case LFRFIDWorkerReadDone:
        event.type = LfRfidApp::EventType::ReadEventDone;
        break;
    }

    event.payload.signed_int = protocol;

    app->view_controller.send_event(&event);
}

void LfRfidAppSceneRead::on_enter(LfRfidApp* app, bool /* need_restore */) {
    auto popup = app->view_controller.get<PopupVM>();

    DOLPHIN_DEED(DolphinDeedRfidRead);
    popup->set_header("Reading\nLF RFID", 89, 34, AlignCenter, AlignTop);
    popup->set_icon(0, 3, &I_RFIDDolphinReceive_97x61);

    app->view_controller.switch_to<PopupVM>();
    lfrfid_worker_start_thread(app->lfworker);
    lfrfid_worker_read_start(app->lfworker, LFRFIDWorkerReadTypeAuto, lfrfid_read_callback, app);

    notification_message(app->notification, &sequence_blink_start_magenta);
}

bool LfRfidAppSceneRead::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = true;

    switch(event->type) {
    case LfRfidApp::EventType::ReadEventSenseStart:
    case LfRfidApp::EventType::ReadEventSenseCardStart:
        notification_message(app->notification, &sequence_blink_start_yellow);
        break;
    case LfRfidApp::EventType::ReadEventSenseEnd:
    case LfRfidApp::EventType::ReadEventSenseCardEnd:
        notification_message(app->notification, &sequence_blink_start_magenta);
        break;
    case LfRfidApp::EventType::ReadEventDone:
        app->protocol_id = event->payload.signed_int;
        DOLPHIN_DEED(DolphinDeedRfidReadSuccess);
        notification_message(app->notification, &sequence_success);
        app->scene_controller.switch_to_next_scene(LfRfidApp::SceneType::ReadSuccess);
        break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

void LfRfidAppSceneRead::on_exit(LfRfidApp* app) {
    notification_message(app->notification, &sequence_blink_stop);
    app->view_controller.get<PopupVM>()->clean();
    lfrfid_worker_stop(app->lfworker);
    lfrfid_worker_stop_thread(app->lfworker);
}
