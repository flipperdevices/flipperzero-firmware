#include "../irda-app.hpp"
#include "assets_icons.h"
#include "gui/modules/button_menu.h"
#include "gui/modules/button_panel.h"
#include "../view/irda-app-brut-view.h"
#include "gui/view.h"
#include "irda/irda-app-view-manager.hpp"
#include "irda/scene/irda-app-scene.hpp"


const char* IrdaAppSceneUniversalTV::universal_tv_db_name = "/irda/universal/tv.ir";

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

    return consumed;
}

void IrdaAppSceneUniversalTV::calculate_messages(IrdaApp* app) {
    bool fs_res = false;
    File file;
    fs_res = file_reader->get_fs_api().file.open(&file, universal_tv_db_name, FSAM_READ, FSOM_OPEN_EXISTING);
    if (!fs_res) {
        app->switch_to_previous_scene();
        return;
    }

    pressed_button = ButtonPanelIndexNA;
    for(int i = 0; i < ButtonPanelIndexMAX; ++i) {
        messages[i] = 0;
    }

    while(1) {
        auto message = file_reader->read_message(&file);
        if (!message) break;
        if (!strcmp(message->name, "POWER")) {
            ++messages[ButtonPanelIndexPower];
        } else if (!strcmp(message->name, "MUTE")) {
            ++messages[ButtonPanelIndexMute];
        } else if (!strcmp(message->name, "VOL+")) {
            ++messages[ButtonPanelIndexVolu];
        } else if (!strcmp(message->name, "VOL-")) {
            ++messages[ButtonPanelIndexVold];
        } else if (!strcmp(message->name, "CH+")) {
            ++messages[ButtonPanelIndexChu];
        } else if (!strcmp(message->name, "CH-")) {
            ++messages[ButtonPanelIndexChd];
        }
    }

    file_reader->get_fs_api().file.close(&file);
}

void IrdaAppSceneUniversalTV::on_enter(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    calculate_messages(app);

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

bool IrdaAppSceneUniversalTV::start_bruteforce(IrdaApp* app, ButtonPanelIndex index) {
    furi_assert((index < ButtonPanelIndexMAX) && (index >= 0));
    auto view_manager = app->get_view_manager();
    auto button_panel = view_manager->get_button_panel();
    auto popup_brut = view_manager->get_popup_brut();

    switch(index) {
    case ButtonPanelIndexPower:
        selected_button = "POWER";
        break;
    case ButtonPanelIndexMute:
        selected_button = "MUTE";
        break;
    case ButtonPanelIndexVolu:
        selected_button = "VOL+";
        break;
    case ButtonPanelIndexVold:
        selected_button = "VOL-";
        break;
    case ButtonPanelIndexChu:
        selected_button = "CH+";
        break;
    case ButtonPanelIndexChd:
        selected_button = "CH-";
        break;
    default:
        furi_assert(0);
        break;
    }

    if (messages[index] > 0) {
        printf("START sending %s\n", selected_button);
        popup_brut_set_progress_max(popup_brut, messages[index]);
        bool fs_res = file_reader->get_fs_api().file.open(&file, universal_tv_db_name, FSAM_READ, FSOM_OPEN_EXISTING);
        if (!fs_res) {
            app->switch_to_previous_scene();
            return false;
        }
        button_panel_set_draw_callback(button_panel, popup_brut_draw_callback, popup_brut);
        button_panel_set_input_callback(button_panel, irda_popup_brut_input_callback, app);
        return true;
    }

    return false;
}

void IrdaAppSceneUniversalTV::stop_bruteforce(IrdaApp* app) {
    auto view_manager = app->get_view_manager();
    auto button_panel = view_manager->get_button_panel();
    file_reader->get_fs_api().file.close(&file);
    button_panel_set_draw_callback(button_panel, NULL, NULL);
    button_panel_set_input_callback(button_panel, NULL, NULL);
}

bool IrdaAppSceneUniversalTV::on_event(IrdaApp* app, IrdaAppEvent* event) {
    bool consumed = false;

    if(event->type == IrdaAppEvent::Type::Tick) {
        if (pressed_button != ButtonPanelIndexNA) {
            if (!send_next_bruteforce(app)) {
                pressed_button = ButtonPanelIndexNA;
                stop_bruteforce(app);
            }
        }
        consumed = true;
    }

    if (event->type == IrdaAppEvent::Type::ButtonPanelPopupBackPressed) {
        pressed_button = ButtonPanelIndexNA;
        stop_bruteforce(app);
        consumed = true;
    } else if (event->type == IrdaAppEvent::Type::ButtonPanelPressed) {
        pressed_button = static_cast<IrdaAppSceneUniversalTV::ButtonPanelIndex>(event->payload.menu_index);
        start_bruteforce(app, pressed_button);
        consumed = true;
    }

    return consumed;
}

void IrdaAppSceneUniversalTV::on_exit(IrdaApp* app) {
    IrdaAppViewManager* view_manager = app->get_view_manager();
    ButtonPanel* button_panel = view_manager->get_button_panel();

    button_panel_clean(button_panel);
}

// TODO: [FL-1418] replace with timer-chained consequence of messages.
bool IrdaAppSceneUniversalTV::send_next_bruteforce(IrdaApp* app) {
    std::unique_ptr<IrdaAppFileReader::IrdaFileMessage> message;

    do {
        message = file_reader->read_message(&file);
        if (message && strcmp(message->name, selected_button))
            printf("----> SKIP : (%s) A:%02lX, C:%02lX\n", message->name, message->message.address, message->message.command);
    } while (message && strcmp(message->name, selected_button));

    if (message) {
        printf("send message: (%s) A:%02lX, C:%02lX\n", message->name, message->message.address, message->message.command);
        auto receiver = app->get_receiver();
        receiver->send_message(&message->message);
        popup_brut_increase_progress(app->get_view_manager()->get_popup_brut());
        auto button_panel = app->get_view_manager()->get_button_panel();
        with_view_model_cpp(button_panel_get_view(button_panel), void*, model, {
            return true;
        });

    }
    return !!message;
}

