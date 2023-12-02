#include "../tpms_app_i.h"
#include "../helpers/tpms_types.h"
#include "../protocols/protocol_items.h"

void tpms_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    TPMSApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void tpms_scene_about_on_enter(void* context) {
    TPMSApp* app = context;

    widget_add_text_box_element(
        app->widget,
        0,
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!          TPMS Reader         \e!\n",
        false);

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");

    furi_string_cat_printf(temp_str, "Version: %s\n", TPMS_VERSION_APP);
    furi_string_cat_printf(temp_str, "Developed by: %s\n", TPMS_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", TPMS_GITHUB);

    furi_string_cat_printf(temp_str, "\e#%s\n", "Description");
    furi_string_cat_printf(
        temp_str, "Reading messages from\nTPMS sensors that work\nwith SubGhz sensors\n\n");

    furi_string_cat_printf(temp_str, "Supported protocols:\n");

    for(size_t i = 0; i < subghz_protocol_registry_count(&tpms_protocol_registry); ++i) {
        char* frequency = NULL;
        char* modulation = NULL;
        const SubGhzProtocol* protocol =
            subghz_protocol_registry_get_by_index(&tpms_protocol_registry, i);

        if((protocol->flag & SubGhzProtocolFlag_433) &&
           (protocol->flag & SubGhzProtocolFlag_315)) {
            frequency = "433|315";
        } else if(protocol->flag & SubGhzProtocolFlag_433) {
            frequency = "433";
        } else if(protocol->flag & SubGhzProtocolFlag_315) {
            frequency = "315";
        }

        if(protocol->flag & SubGhzProtocolFlag_AM) {
            modulation = "AM";
        } else if(protocol->flag & SubGhzProtocolFlag_FM) {
            modulation = "FM";
        }
        furi_string_cat_printf(temp_str, "%s (%s %s)\n", protocol->name, frequency, modulation);
    }

    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, TPMSViewWidget);
}

bool tpms_scene_about_on_event(void* context, SceneManagerEvent event) {
    TPMSApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void tpms_scene_about_on_exit(void* context) {
    TPMSApp* app = context;

    // Clear views
    widget_reset(app->widget);
}
