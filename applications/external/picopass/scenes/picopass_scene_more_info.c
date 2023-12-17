#include "../picopass_i.h"
#include <dolphin/dolphin.h>

void picopass_scene_more_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    Picopass* picopass = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(picopass->view_dispatcher, result);
    }
}

void picopass_scene_more_info_on_enter(void* context) {
    Picopass* picopass = context;
    PicopassBlock* AA1 = picopass->dev->dev_data.AA1;

    furi_string_reset(picopass->text_box_store);

    size_t app_limit = MIN(AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[0], PICOPASS_MAX_APP_LIMIT);
    FuriString* str = picopass->text_box_store;

    for(size_t i = 0; i < app_limit; i++) {
        for(size_t j = 0; j < PICOPASS_BLOCK_LEN; j += 2) {
            furi_string_cat_printf(str, "%02X%02X ", AA1[i].data[j], AA1[i].data[j + 1]);
        }
    }

    text_box_set_font(picopass->text_box, TextBoxFontHex);
    text_box_set_text(picopass->text_box, furi_string_get_cstr(picopass->text_box_store));
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewTextBox);
}

bool picopass_scene_more_info_on_event(void* context, SceneManagerEvent event) {
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

void picopass_scene_more_info_on_exit(void* context) {
    Picopass* picopass = context;

    // Clear views
    widget_reset(picopass->widget);
}
