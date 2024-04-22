#include "../seader_i.h"
#include <dolphin/dolphin.h>

void seader_scene_formats_on_enter(void* context) {
    Seader* seader = context;
    PluginWiegand* plugin = seader->plugin_wiegand;
    SeaderCredential* credential = seader->credential;

    FuriString* str = seader->text_box_store;
    furi_string_reset(str);

    if(plugin) {
        FuriString* description = furi_string_alloc();
        size_t format_count = plugin->count(credential->bit_length, credential->credential);
        for(size_t i = 0; i < format_count; i++) {
            plugin->description(credential->bit_length, credential->credential, i, description);

            furi_string_cat_printf(str, "%s\n", furi_string_get_cstr(description));
        }
        furi_string_free(description);
    }

    text_box_set_font(seader->text_box, TextBoxFontHex);
    text_box_set_text(seader->text_box, furi_string_get_cstr(seader->text_box_store));
    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewTextBox);
}

bool seader_scene_formats_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(seader->scene_manager);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(seader->scene_manager);
    }
    return consumed;
}

void seader_scene_formats_on_exit(void* context) {
    Seader* seader = context;

    // Clear views
    text_box_reset(seader->text_box);
}
