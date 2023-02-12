#include "../ibutton_i.h"
#include <toolbox/path.h>

void ibutton_scene_info_on_enter(void* context) {
    iButton* ibutton = context;
    iButtonKey* key = ibutton->key;
    Widget* widget = ibutton->widget;

    FuriString* tmp = furi_string_alloc();

    furi_string_printf(tmp, "%s [%s]", ibutton->key_name, ibutton_key_get_protocol_name(key));

    widget_add_string_element(
        widget, 0, 2, AlignLeft, AlignTop, FontPrimary, furi_string_get_cstr(tmp));

    furi_string_reset(tmp);
    ibutton_key_get_rendered_brief_data(key, tmp);

    widget_add_string_multiline_element(
        widget, 0, 16, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(tmp));

    if(ibutton_key_get_features(key) & iButtonProtocolFeatureExtData) {
        widget_add_button_element(
            widget, GuiButtonTypeRight, "More", ibutton_widget_callback, context);
    }

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewWidget);
    furi_string_free(tmp);
}

bool ibutton_scene_info_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneViewData);
        }
    }

    return consumed;
}

void ibutton_scene_info_on_exit(void* context) {
    iButton* ibutton = context;
    widget_reset(ibutton->widget);
}
