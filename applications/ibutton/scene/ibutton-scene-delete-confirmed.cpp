#include "ibutton-scene-delete-confirmed.h"
#include "../ibutton-app.h"
#include "../ibutton-view-manager.h"
#include "../ibutton-event.h"
#include "../ibutton-key.h"

void iButtonSceneDeleteConfirmed::on_enter(iButtonApp* app) {
    iButtonAppViewManager* view_manager = app->get_view_manager();
    Popup* popup = view_manager->get_popup();

    popup_set_icon(popup, 0, 2, I_DolphinMafia_115x62);
    popup_set_text(popup, "Deleted", 83, 19, AlignLeft, AlignBottom);

    view_manager->switch_to(iButtonAppViewManager::Type::iButtonAppViewPopup);
    app->get_onewire_master()->start();
}

bool iButtonSceneDeleteConfirmed::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeBack) {
        app->switch_to_prevous_scene(3);
        consumed = true;
    }

    return consumed;
}

void iButtonSceneDeleteConfirmed::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view_manager()->get_popup();

    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);

    app->get_onewire_master()->stop();
}