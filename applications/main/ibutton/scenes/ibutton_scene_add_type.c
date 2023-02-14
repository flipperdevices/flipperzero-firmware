#include "../ibutton_i.h"

void ibutton_scene_add_type_on_enter(void* context) {
    iButton* ibutton = context;
    Submenu* submenu = ibutton->submenu;

    FuriString* tmp = furi_string_alloc();

    for(iButtonProtocol protocol_id = 0; protocol_id < iButtonProtocolMax; ++protocol_id) {
        furi_string_printf(
            tmp,
            "%s %s",
            ibutton_protocols_get_manufacturer(protocol_id),
            ibutton_protocols_get_name(protocol_id));

        submenu_add_item(
            submenu, furi_string_get_cstr(tmp), protocol_id, ibutton_submenu_callback, context);
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(ibutton->scene_manager, iButtonSceneAddType));

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewSubmenu);
    furi_string_free(tmp);
}

bool ibutton_scene_add_type_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        scene_manager_set_scene_state(ibutton->scene_manager, iButtonSceneAddType, event.event);

        // TODO: set the key protocol

        scene_manager_next_scene(ibutton->scene_manager, iButtonSceneAddValue);
    }

    return consumed;
}

void ibutton_scene_add_type_on_exit(void* context) {
    iButton* ibutton = context;
    submenu_reset(ibutton->submenu);
}
