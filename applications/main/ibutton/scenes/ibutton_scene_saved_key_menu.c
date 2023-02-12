#include "../ibutton_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexWriteBlank,
    SubmenuIndexWriteCopy,
    SubmenuIndexEdit,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
};

void ibutton_scene_saved_key_menu_submenu_callback(void* context, uint32_t index) {
    iButton* ibutton = context;
    view_dispatcher_send_custom_event(ibutton->view_dispatcher, index);
}

void ibutton_scene_saved_key_menu_on_enter(void* context) {
    iButton* ibutton = context;
    Submenu* submenu = ibutton->submenu;

    const uint32_t features = ibutton_key_get_features(ibutton->key);

    submenu_add_item(
        submenu,
        "Emulate",
        SubmenuIndexEmulate,
        ibutton_scene_saved_key_menu_submenu_callback,
        ibutton);

    if(features & iButtonProtocolFeatureWriteBlank) {
        submenu_add_item(submenu, "Write Blank", SubmenuIndexWriteBlank, ibutton_scene_saved_key_menu_submenu_callback, ibutton);
    }

    if(features & iButtonProtocolFeatureWriteCopy) {
        submenu_add_item(submenu, "Write Copy", SubmenuIndexWriteCopy, ibutton_scene_saved_key_menu_submenu_callback, ibutton);
    }

    submenu_add_item(
        submenu, "Edit", SubmenuIndexEdit, ibutton_scene_saved_key_menu_submenu_callback, ibutton);

    submenu_add_item(
        submenu,
        "Delete",
        SubmenuIndexDelete,
        ibutton_scene_saved_key_menu_submenu_callback,
        ibutton);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, ibutton_scene_saved_key_menu_submenu_callback, ibutton);

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewSubmenu);
}

bool ibutton_scene_saved_key_menu_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneEmulate);
            DOLPHIN_DEED(DolphinDeedIbuttonEmulate);
        } else if(event.event == SubmenuIndexWriteBlank) {
            // TODO: Write blank
        } else if(event.event == SubmenuIndexWriteCopy) {
            // TODO: Write copy
        } else if(event.event == SubmenuIndexEdit) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneAddValue);
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneDeleteConfirm);
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneInfo);
        }
    }

    return consumed;
}

void ibutton_scene_saved_key_menu_on_exit(void* context) {
    iButton* ibutton = context;
    submenu_reset(ibutton->submenu);
}
