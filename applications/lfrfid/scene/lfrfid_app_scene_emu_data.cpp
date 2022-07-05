#include "lfrfid_app_scene_emu_data.h"
#include <dolphin/dolphin.h>

void LfRfidAppSceneEmuData::on_enter(LfRfidApp* app, bool /*need_restore*/) {
    auto byte_input = app->view_controller.get<ByteInputVM>();

    byte_input->set_header_text("Enter the data in hex");

    byte_input->set_result_callback(
        save_callback, NULL, app, key_data, app->worker.key.get_type_data_count());

    app->view_controller.switch_to<ByteInputVM>();
}

bool LfRfidAppSceneEmuData::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;
    RfidKey& key = app->worker.key;

    if(event->type == LfRfidApp::EventType::Next) {
        key.set_data(key_data, key.get_type_data_count());
        DOLPHIN_DEED(DolphinDeedRfidAdd);
        app->scene_controller.switch_to_next_scene(LfRfidApp::SceneType::Emulate);
    }

    return consumed;
}

void LfRfidAppSceneEmuData::on_exit(LfRfidApp* app) {
    app->view_controller.get<ByteInputVM>()->clean();
}

void LfRfidAppSceneEmuData::save_callback(void* context) {
    LfRfidApp* app = static_cast<LfRfidApp*>(context);
    LfRfidApp::Event event;
    event.type = LfRfidApp::EventType::Next;
    app->view_controller.send_event(&event);
}