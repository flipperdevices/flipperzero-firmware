#include "ibutton-scene-save-confirmed.h"
#include "../ibutton-app.h"
#include "../ibutton-view-manager.h"
#include "../ibutton-event.h"
#include "../ibutton-key.h"

void iButtonSceneSaveConfirmed::on_enter(iButtonApp* app) {
    iButtonAppViewManager* view_manager = app->get_view_manager();
    Popup* popup = view_manager->get_popup();

    popup_set_icon(popup, 32, 5, I_DolphinNice_96x59);
    popup_set_text(popup, "Saved", 13, 22, AlignLeft, AlignBottom);

    view_manager->switch_to(iButtonAppViewManager::Type::iButtonAppViewPopup);
}

bool iButtonSceneSaveConfirmed::on_event(iButtonApp* app, iButtonEvent* event) {
    bool consumed = false;

    if(event->type == iButtonEvent::Type::EventTypeBack) {
        app->switch_to_prevous_scene(2);
        consumed = true;
    }

    return consumed;
}

void iButtonSceneSaveConfirmed::on_exit(iButtonApp* app) {
    Popup* popup = app->get_view_manager()->get_popup();

    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignTop);
    popup_set_icon(popup, -1, -1, I_DolphinWait_61x59);
}