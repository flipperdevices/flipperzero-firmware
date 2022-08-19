#include "lfrfid_app_scene_raw_info.h"
#include "../view/elements/button_element.h"
#include "../view/elements/icon_element.h"
#include "../view/elements/string_element.h"

void LfRfidAppSceneRawInfo::on_enter(LfRfidApp* app, bool /* need_restore */) {
    string_init(string_info);

    string_printf(
        string_info,
        "RAW RFID data reader\r\n"
        "1) Put the Flipper on your card\r\n"
        "2) Press OK\r\n"
        "3) Wait until data is read");

    auto container = app->view_controller.get<ContainerVM>();

    auto line = container->add<StringElement>();
    line->set_text(string_get_cstr(string_info), 0, 1, 0, AlignLeft, AlignTop, FontSecondary);

    auto button = container->add<ButtonElement>();
    button->set_type(ButtonElement::Type::Center, "OK");
    button->set_callback(app, LfRfidAppSceneRawInfo::ok_callback);

    app->view_controller.switch_to<ContainerVM>();
}

bool LfRfidAppSceneRawInfo::on_event(LfRfidApp* app, LfRfidApp::Event* event) {
    bool consumed = false;
    if(event->type == LfRfidApp::EventType::Next) {
        app->scene_controller.switch_to_scene({LfRfidApp::SceneType::RawRead});
        consumed = true;
    }
    return consumed;
}

void LfRfidAppSceneRawInfo::on_exit(LfRfidApp* app) {
    app->view_controller.get<ContainerVM>()->clean();
    string_clear(string_info);
}

void LfRfidAppSceneRawInfo::ok_callback(void* context) {
    LfRfidApp* app = static_cast<LfRfidApp*>(context);
    LfRfidApp::Event event;
    event.type = LfRfidApp::EventType::Next;
    app->view_controller.send_event(&event);
}
