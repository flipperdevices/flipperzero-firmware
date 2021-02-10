#include "ibutton-scene-read.h"
#include "../ibutton-app.h"
#include "../ibutton-view-manager.h"
#include "../ibutton-event.h"

void iButtonSceneRead::on_enter(iButtonApp* app) {
    iButtonAppViewManager* view_manager = app->get_view_manager();
    Popup* popup = view_manager->get_popup();

    popup_set_header(popup, "iButton", 95, 26, AlignCenter, AlignBottom);
    popup_set_text(popup, "waiting\nfor key ...", 95, 30, AlignCenter, AlignTop);
    popup_set_icon(popup, 0, 5, I_DolphinWait_61x59);

    view_manager->switch_to(iButtonAppViewManager::Type::iButtonAppViewPopup);
    app->get_onewire_master()->start();
}

bool iButtonSceneRead::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeTick) {
        bool result = 0;
        const uint8_t key_size = app->get_key()->get_size();
        uint8_t address[key_size];
        uint8_t address_second[key_size];
        OneWireMaster* onewire = app->get_onewire_master();

        consumed = true;
        app->notify_red_blink();

        app->pause_os();
        result = onewire->reset();
        app->resume_os();

        if(result) {
            app->pause_os();
            onewire->write(0x33);
            onewire->read_bytes(address, key_size);
            app->resume_os();

            if(maxim_crc8(address, key_size) == 0) {
                if(address[0] == 0x01) {
                    app->get_key()->set_data(address, key_size);
                    app->switch_to_next_scene(iButtonApp::Scene::SceneReadSuccess);
                } else {
                    // not are key error
                    app->get_key()->set_data(address, key_size);
                    app->switch_to_next_scene(iButtonApp::Scene::SceneReadNotKeyError);
                }
            } else {
                // read twice, if keys are same - we get crc error
                delay(100);

                app->pause_os();
                result = onewire->reset();
                app->resume_os();

                if(result) {
                    app->pause_os();
                    onewire->write(0x33);
                    onewire->read_bytes(address_second, key_size);
                    app->resume_os();

                    if(memcmp(address, address_second, key_size) == 0) {
                        // crc error
                        app->get_key()->set_data(address, key_size);
                        app->switch_to_next_scene(iButtonApp::Scene::SceneReadCRCError);
                    }
                }
            }
        }
    }

    return consumed;
}

void iButtonSceneRead::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view_manager()->get_popup();

    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignBottom);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);

    app->get_onewire_master()->stop();
}