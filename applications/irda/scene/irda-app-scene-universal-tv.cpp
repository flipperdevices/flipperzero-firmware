#include "../irda-app.hpp"
#include "assets_icons.h"
#include "gui/modules/button_panel.h"
#include "../view/irda-app-brut-view.h"
#include "irda/irda-app-view-manager.hpp"

typedef enum {
    ButtonPanelIndexPower,
    ButtonPanelIndexMute,
    ButtonPanelIndexChu,
    ButtonPanelIndexChd,
    ButtonPanelIndexVolu,
    ButtonPanelIndexVold,
} ButtonPanelIndex;

static void irda_app_tv_button_item_callback(void* context, uint32_t index) {
    IrdaApp* app = static_cast<IrdaApp*>(context);
    IrdaAppEvent event;

    event.type = IrdaAppEvent::Type::ButtonPanelPressed;
    event.payload.menu_index = index;

    app->get_view_manager()->send_event(&event);
}

static bool irda_popup_brut_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);
    auto app = static_cast<IrdaApp*>(context);
    bool consumed = false;

    if ((event->type == InputTypeShort) && (event->key == InputKeyBack)) {
        consumed = true;
        IrdaAppEvent irda_event;

        irda_event.type = IrdaAppEvent::Type::ButtonPanelPopupBackPressed;
        app->get_view_manager()->send_event(&irda_event);
    }

//    static int n = 0;
//    if (++n > 10) {
//        popup_brut_increase_progress(popup_brut);
//        n = 0;
//    }
    return consumed;
}

void IrdaAppSceneUniversalTV::on_enter(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    button_panel_add_item(button_panel, ButtonPanelIndexPower,  0, 0, 3, 19, I_Power_25x27, I_Power_hvr_25x27, irda_app_tv_button_item_callback, app);
    button_panel_add_item(button_panel, ButtonPanelIndexMute,   1, 0, 36, 19, I_Mute_25x27, I_Mute_hvr_25x27, irda_app_tv_button_item_callback, app);
    button_panel_add_item(button_panel, ButtonPanelIndexChu,    0, 1, 3, 66, I_Up_25x27, I_Up_hvr_25x27, irda_app_tv_button_item_callback, app);
    button_panel_add_item(button_panel, ButtonPanelIndexVolu,   1, 1, 36, 66, I_Vol_up_25x27, I_Vol_up_hvr_25x27, irda_app_tv_button_item_callback, app);
    button_panel_add_item(button_panel, ButtonPanelIndexChd,    0, 2, 3, 98, I_Down_25x27, I_Down_hvr_25x27, irda_app_tv_button_item_callback, app);
    button_panel_add_item(button_panel, ButtonPanelIndexVold,   1, 2, 36, 98, I_Vol_down_25x27, I_Vol_down_hvr_25x27, irda_app_tv_button_item_callback, app);

    button_panel_add_label(button_panel, 6, 11, FontPrimary, "TV remote");
    button_panel_add_label(button_panel, 9, 64, FontSecondary, "Vol");
    button_panel_add_label(button_panel, 43, 64, FontSecondary, "Ch");
    view_manager->switch_to(IrdaAppViewManager::ViewType::ButtonPanel);
}

bool IrdaAppSceneUniversalTV::on_event(IrdaApp* app, IrdaAppEvent* event) {
    bool consumed = false;

    if(event->type == IrdaAppEvent::Type::Tick) {
        static int a = 0;
        if (!(++a % 10))
            popup_brut_increase_progress(app->get_view_manager()->get_popup_brut());
        consumed = true;
    }

    auto view_manager = app->get_view_manager();
    if (event->type == IrdaAppEvent::Type::ButtonPanelPopupBackPressed) {
        button_panel_set_draw_callback(view_manager->get_button_panel(), NULL, NULL);
        button_panel_set_input_callback(view_manager->get_button_panel(), NULL, NULL);
    } else if (event->type == IrdaAppEvent::Type::ButtonPanelPressed) {
        switch(event->payload.menu_index) {
        case ButtonPanelIndexPower:
            popup_brut_set_progress_max(view_manager->get_popup_brut(), 100 /*tmp*/);      // read file and set number of messages
            button_panel_set_draw_callback(view_manager->get_button_panel(), popup_brut_draw_callback, view_manager->get_popup_brut());
            button_panel_set_input_callback(view_manager->get_button_panel(), irda_popup_brut_input_callback, app);
            printf("Power\r\n");
            consumed = true;
            break;
        case ButtonPanelIndexMute:
            break;
        case ButtonPanelIndexChu:
            break;
        case ButtonPanelIndexChd:
            break;
        case ButtonPanelIndexVolu:
            break;
        case ButtonPanelIndexVold:
            break;
        default:
            furi_assert(0);
            break;
        }
    }

    return consumed;
}

void IrdaAppSceneUniversalTV::on_exit(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    button_panel_clean(button_panel);
}
