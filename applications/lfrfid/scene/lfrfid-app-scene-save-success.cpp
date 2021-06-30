#include "lfrfid-app-scene-save-success.h"

void LfRfidAppSceneSaveSuccess::on_enter(LfRfidApp* app, bool need_restore) {
    auto popup = app->view_controller.get<PopupVM>();

    popup->set_icon(32, 5, I_DolphinNice_96x59);
    popup->set_text("Saved!", 13, 22, AlignLeft, AlignBottom);
    popup->set_context(app);
    popup->set_callback(LfRfidAppSceneSaveSuccess::timeout_callback);
    popup->set_timeout(1500);
    popup->enable_timeout();

    app->view_controller.switch_to<PopupVM>();
    notification_message_block(app->notification, &sequence_set_green_255);
}

bool LfRfidAppSceneSaveSuccess::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;

    if(event->type == LfRfidApp::EventType::Back) {
        bool result = app->scene_controller.has_previous_scene(
            {LfRfidApp::SceneType::ReadedMenu, LfRfidApp::SceneType::SavedKeyMenu});

        if(result) {
            app->scene_controller.search_and_switch_to_previous_scene(
                {LfRfidApp::SceneType::ReadedMenu, LfRfidApp::SceneType::SavedKeyMenu});
        } else {
            app->scene_controller.search_and_switch_to_another_scene(
                {LfRfidApp::SceneType::SaveType}, LfRfidApp::SceneType::SavedKeyMenu);
        }

        consumed = true;
    }

    return consumed;
}

void LfRfidAppSceneSaveSuccess::on_exit(LfRfidApp* app) {
    notification_message_block(app->notification, &sequence_reset_green);
    app->view_controller.get<PopupVM>()->clean();
}

void LfRfidAppSceneSaveSuccess::timeout_callback(void* context) {
    LfRfidApp* app = static_cast<LfRfidApp*>(context);
    LfRfidApp::Event event;
    event.type = LfRfidApp::EventType::Back;
    app->view_controller.send_event(&event);
}