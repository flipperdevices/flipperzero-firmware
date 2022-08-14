#include "lfrfid_app_scene_read_success.h"
#include "../view/elements/button_element.h"
#include "../view/elements/icon_element.h"
#include "../view/elements/string_element.h"

void LfRfidAppSceneReadSuccess::on_enter(LfRfidApp* app, bool /* need_restore */) {
    auto container = app->view_controller.get<ContainerVM>();

    auto button = container->add<ButtonElement>();
    button->set_type(ButtonElement::Type::Left, "Retry");
    button->set_callback(app, LfRfidAppSceneReadSuccess::back_callback);

    button = container->add<ButtonElement>();
    button->set_type(ButtonElement::Type::Right, "More");
    button->set_callback(app, LfRfidAppSceneReadSuccess::more_callback);

    app->view_controller.switch_to<ContainerVM>();

    notification_message_block(app->notification, &sequence_set_green_255);
}

bool LfRfidAppSceneReadSuccess::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;

    if(event->type == LfRfidApp::EventType::Next) {
        app->scene_controller.switch_to_next_scene(LfRfidApp::SceneType::ReadKeyMenu);
        consumed = true;
    } else if(event->type == LfRfidApp::EventType::Retry) {
        app->scene_controller.switch_to_next_scene({LfRfidApp::SceneType::RetryConfirm});
        consumed = true;
    } else if(event->type == LfRfidApp::EventType::Back) {
        app->scene_controller.switch_to_next_scene({LfRfidApp::SceneType::ExitConfirm});
        consumed = true;
    }

    return consumed;
}

void LfRfidAppSceneReadSuccess::on_exit(LfRfidApp* app) {
    notification_message_block(app->notification, &sequence_reset_green);
    app->view_controller.get<ContainerVM>()->clean();
}

void LfRfidAppSceneReadSuccess::back_callback(void* context) {
    LfRfidApp* app = static_cast<LfRfidApp*>(context);
    LfRfidApp::Event event;
    event.type = LfRfidApp::EventType::Retry;
    app->view_controller.send_event(&event);
}

void LfRfidAppSceneReadSuccess::more_callback(void* context) {
    LfRfidApp* app = static_cast<LfRfidApp*>(context);
    LfRfidApp::Event event;
    event.type = LfRfidApp::EventType::Next;
    app->view_controller.send_event(&event);
}
