#include "scened-app-scene-start.h"

typedef enum {
    SubmenuIndexOne,
} SubmenuIndex;

void ScenedAppSceneStart::on_enter(ScenedApp* app, bool need_restore) {
    SubmenuM* submenu = app->view_controller.get<SubmenuM>();
    auto callback = cbc::obtain_connector(this, &ScenedAppSceneStart::submenu_callback);

    submenu->add_item("Read", SubmenuIndexOne, callback, app);

    if(need_restore) {
        submenu->set_selected_item(submenu_item_selected);
    }
    app->view_controller.switch_to<SubmenuM>();
}

bool ScenedAppSceneStart::on_event(ScenedApp* app, ScenedApp::Event* event) {
    bool consumed = false;

    if(event->type == ScenedApp::EventType::MenuSelected) {
        submenu_item_selected = event->payload.menu_index;
        switch(event->payload.menu_index) {
        case SubmenuIndexOne:
            break;
        }
        consumed = true;
    }

    return consumed;
}

void ScenedAppSceneStart::on_exit(ScenedApp* app) {
}

void ScenedAppSceneStart::submenu_callback(void* context, uint32_t index) {
    ScenedApp* app = static_cast<ScenedApp*>(context);
    ScenedApp::Event event;

    event.type = ScenedApp::EventType::MenuSelected;
    event.payload.menu_index = index;

    app->view_controller.send_event(&event);
}
