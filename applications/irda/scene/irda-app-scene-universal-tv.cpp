#include "../irda-app.hpp"
#include "assets_icons.h"
#include "gui/modules/button_panel.h"

typedef enum {
    ButtonPanelIndexPower,
    ButtonPanelIndexMute,
    ButtonPanelIndexChu,
    ButtonPanelIndexChd,
    ButtonPanelIndexVolu,
    ButtonPanelIndexVold,
} ButtonPanelIndex;

void IrdaAppSceneUniversalTV::on_enter(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    button_panel_add_item(button_panel, ButtonPanelIndexPower,  0, 0, 6, 15, I_Power_25x27, I_Power_hvr_25x27, NULL, NULL);
    button_panel_add_item(button_panel, ButtonPanelIndexMute,   1, 0, 35, 15, I_Mute_25x27, I_Mute_hvr_25x27, NULL, NULL);
    button_panel_add_item(button_panel, ButtonPanelIndexChu,    0, 1, 6, 68, I_Up_25x27, I_Up_hvr_25x27, NULL, NULL);
    button_panel_add_item(button_panel, ButtonPanelIndexVolu,   1, 1, 35, 68, I_Vol_up_25x27, I_Vol_up_hvr_25x27, NULL, NULL);
    button_panel_add_item(button_panel, ButtonPanelIndexChd,    0, 2, 6, 100, I_Down_25x27, I_Down_hvr_25x27, NULL, NULL);
    button_panel_add_item(button_panel, ButtonPanelIndexVold,   1, 2, 35, 100, I_Vol_down_25x27, I_Vol_down_hvr_25x27, NULL, NULL);

    view_manager->switch_to(IrdaAppViewManager::ViewType::ButtonPanel);
}

bool IrdaAppSceneUniversalTV::on_event(IrdaApp* app, IrdaAppEvent* event) {
    bool consumed = false;

    if(event->type == IrdaAppEvent::Type::MenuSelected) {
        consumed = true;
    }

    return consumed;
}

void IrdaAppSceneUniversalTV::on_exit(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    button_panel_clean(button_panel);
}
