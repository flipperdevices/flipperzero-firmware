#include "lf-rfid-scene-start.h"
#include "../lf-rfid-app.h"
#include "../lf-rfid-view-manager.h"
#include "../lf-rfid-event.h"
#include <callback-connector.h>

typedef enum {
    SubmenuIndexRead,
    SubmenuIndexEmulateEM,
    SubmenuIndexEmulateHID,
    SubmenuIndexTune
} SubmenuIndex;

void LfrfidSceneStart::on_enter(LfrfidApp* app) {
    LfrfidAppViewManager* view_manager = app->get_view_manager();
    Submenu* submenu = view_manager->get_submenu();
    auto callback = cbc::obtain_connector(this, &LfrfidSceneStart::submenu_callback);

    submenu_add_item(submenu, "Read", SubmenuIndexRead, callback, app);
    submenu_add_item(submenu, "Emulate EM", SubmenuIndexEmulateEM, callback, app);
    submenu_add_item(submenu, "Emulate HID", SubmenuIndexEmulateHID, callback, app);
    submenu_add_item(submenu, "Tune", SubmenuIndexTune, callback, app);

    view_manager->switch_to(LfrfidAppViewManager::ViewType::Submenu);
}

bool LfrfidSceneStart::on_event(LfrfidApp* app, LfrfidEvent* event) {
    bool consumed = false;

    if(event->type == LfrfidEvent::Type::MenuSelected) {
        switch(event->payload.menu_index) {
        case SubmenuIndexRead:
            app->switch_to_next_scene(LfrfidApp::Scene::Read);
            break;
        case SubmenuIndexEmulateEM:
            app->notify_green_on();
            emulator.start();
            emulator.emulate(LfrfidKeyType::KeyEmarine, nullptr, 0);
            emulator.stop();
            app->notify_green_off();
            break;
        case SubmenuIndexEmulateHID:
            app->notify_green_on();
            emulator.start();
            emulator.emulate(LfrfidKeyType::KeyHID, nullptr, 0);
            emulator.stop();
            app->notify_green_off();
            break;
        case SubmenuIndexTune:
            app->switch_to_next_scene(LfrfidApp::Scene::Tune);
            break;
        }
        consumed = true;
    }

    /*if(event->type == LfrfidEvent::Type::Tick) {
        emulator.emulate(LfrfidKeyType::KeyEmarine, nullptr, 0);
    }*/

    return consumed;
}

void LfrfidSceneStart::on_exit(LfrfidApp* app) {
    LfrfidAppViewManager* view_manager = app->get_view_manager();
    Submenu* submenu = view_manager->get_submenu();

    submenu_clean(submenu);
}

void LfrfidSceneStart::submenu_callback(void* context, uint32_t index) {
    LfrfidApp* app = static_cast<LfrfidApp*>(context);
    LfrfidEvent event;

    event.type = LfrfidEvent::Type::MenuSelected;
    event.payload.menu_index = index;

    app->get_view_manager()->send_event(&event);
}