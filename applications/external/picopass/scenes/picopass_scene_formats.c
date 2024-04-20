#include "../picopass_i.h"
#include <dolphin/dolphin.h>

void picopass_scene_formats_on_enter(void* context) {
    Picopass* picopass = context;
    PluginWiegand* plugin = picopass->plugin_wiegand;
    PicopassDevice* dev = picopass->dev;
    PicopassDeviceData dev_data = dev->dev_data;
    PicopassPacs pacs = dev_data.pacs;

    FuriString* str = picopass->text_box_store;
    furi_string_reset(str);

    // Convert from byte array to uint64_t
    uint64_t credential = 0;
    memcpy(&credential, pacs.credential, sizeof(pacs.credential));
    credential = __builtin_bswap64(credential);

    if(plugin) {
        FuriString* description = furi_string_alloc();
        size_t format_count = plugin->count(pacs.bitLength, credential);
        for(size_t i = 0; i < format_count; i++) {
            plugin->description(pacs.bitLength, credential, i, description);

            furi_string_cat_printf(str, "%s\n", furi_string_get_cstr(description));
        }
        furi_string_free(description);
    }

    text_box_set_font(picopass->text_box, TextBoxFontHex);
    text_box_set_text(picopass->text_box, furi_string_get_cstr(picopass->text_box_store));
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewTextBox);
}

bool picopass_scene_formats_on_event(void* context, SceneManagerEvent event) {
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

void picopass_scene_formats_on_exit(void* context) {
    Picopass* picopass = context;

    // Clear views
    text_box_reset(picopass->text_box);
}
