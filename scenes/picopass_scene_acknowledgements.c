#include "../picopass_i.h"
#include <dolphin/dolphin.h>
#include "../acknowledgements.h"

void picopass_scene_acknowledgements_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    Picopass* picopass = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(picopass->view_dispatcher, result);
    }
}

void picopass_scene_acknowledgements_on_enter(void* context) {
    Picopass* picopass = context;

    furi_string_reset(picopass->text_box_store);

    FuriString* str = picopass->text_box_store;
    furi_string_cat_printf(str, "%s\n", acknowledgements_text);

    text_box_set_font(picopass->text_box, TextBoxFontText);
    text_box_set_text(picopass->text_box, furi_string_get_cstr(picopass->text_box_store));
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewTextBox);
}

bool picopass_scene_acknowledgements_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(picopass->scene_manager);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(picopass->scene_manager);
    }
    return consumed;
}

void picopass_scene_acknowledgements_on_exit(void* context) {
    Picopass* picopass = context;

    // Clear views
    text_box_reset(picopass->text_box);
}
