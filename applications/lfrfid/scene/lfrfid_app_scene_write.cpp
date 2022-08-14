#include "lfrfid_app_scene_write.h"

static void lfrfid_write_callback(LFRFIDWorkerWriteResult result, void* ctx) {
    LfRfidApp* app = static_cast<LfRfidApp*>(ctx);
    LfRfidApp::Event event;

    event.type = LfRfidApp::EventType::EmulateEvent;
    event.payload.payload = result;

    app->view_controller.send_event(&event);
}

void LfRfidAppSceneWrite::on_enter(LfRfidApp* app, bool /* need_restore */) {
    auto popup = app->view_controller.get<PopupVM>();

    popup->set_header("Writing", 89, 30, AlignCenter, AlignTop);
    if(string_size(app->file_name)) {
        popup->set_text(string_get_cstr(app->file_name), 89, 43, AlignCenter, AlignTop);
    } else {
        popup->set_text(
            protocol_dict_get_name(app->dict, app->protocol_id), 89, 43, AlignCenter, AlignTop);
    }
    popup->set_icon(0, 3, &I_RFIDDolphinSend_97x61);

    app->view_controller.switch_to<PopupVM>();
    lfrfid_worker_write_start(
        app->lfworker, (LFRFIDProtocol)app->protocol_id, lfrfid_write_callback, app);
}

bool LfRfidAppSceneWrite::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;

    if(event->type == LfRfidApp::EventType::EmulateEvent) {
        consumed = true;
        auto popup = app->view_controller.get<PopupVM>();

        switch(event->payload.payload) {
        case LFRFIDWorkerWriteOK:
            notification_message(app->notification, &sequence_success);
            app->scene_controller.switch_to_next_scene(LfRfidApp::SceneType::WriteSuccess);
            break;
        case LFRFIDWorkerWriteProtocolCannotBeWritten:
            popup->set_icon(72, 14, &I_DolphinFirstStart8_56x51);
            popup->set_header("Error", 64, 3, AlignCenter, AlignTop);
            popup->set_text("This protocol\ncannot be written", 3, 17, AlignLeft, AlignTop);
            notification_message(app->notification, &sequence_blink_yellow_10);
            break;
        case LFRFIDWorkerWriteTooLongToWrite:
        case LFRFIDWorkerWriteFobCannotBeWritten:
            popup->set_icon(72, 14, &I_DolphinFirstStart8_56x51);
            popup->set_header("Still trying to write...", 64, 3, AlignCenter, AlignTop);
            popup->set_text(
                "Make sure this\ncard is writable\nand not\nprotected.",
                3,
                17,
                AlignLeft,
                AlignTop);
            notification_message(app->notification, &sequence_blink_yellow_10);
            break;
        default:
            consumed = false;
        }
    }

    if(!consumed) {
        notification_message(app->notification, &sequence_blink_magenta_10);
    }

    return consumed;
}

void LfRfidAppSceneWrite::on_exit(LfRfidApp* app) {
    app->view_controller.get<PopupVM>()->clean();
    lfrfid_worker_stop(app->lfworker);
}
