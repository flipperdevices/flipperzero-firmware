#include "ibutton-scene-write.h"
#include "../ibutton-app.h"
#include "../ibutton-view-manager.h"
#include "../ibutton-event.h"
#include "../ibutton-key.h"

void iButtonSceneWrite::on_enter(iButtonApp* app) {
    iButtonAppViewManager* view_manager = app->get_view_manager();
    Popup* popup = view_manager->get_popup();
    iButtonKey* key = app->get_key();
    uint8_t* key_data = key->get_data();
    const char* key_name = key->get_name();

    // check that stored key has name
    if(strcmp(key_name, "") != 0) {
        app->set_text_store("writing\n%s", key_name);
    } else {
        // if not, show key data
        switch(key->get_key_type()) {
        case iButtonKeyType::KeyDallas:
            app->set_text_store(
                "writing\n%02X %02X %02X %02X\n%02X %02X %02X %02X",
                key_data[0],
                key_data[1],
                key_data[2],
                key_data[3],
                key_data[4],
                key_data[5],
                key_data[6],
                key_data[7]);

            break;
        }
    }

    popup_set_header(popup, "iButton", 95, 18, AlignCenter, AlignBottom);
    popup_set_text(popup, app->get_text_store(), 95, 22, AlignCenter, AlignTop);
    popup_set_icon(popup, 10, 10, I_iButtonKey_49x44);

    view_manager->switch_to(iButtonAppViewManager::Type::iButtonAppViewPopup);
    app->get_onewire_master()->start();
}

bool iButtonSceneWrite::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeTick) {
        consumed = true;
        app->notify_red_blink();
    }

    return consumed;
}

void iButtonSceneWrite::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view_manager()->get_popup();

    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignBottom);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);

    app->get_onewire_master()->stop();
}