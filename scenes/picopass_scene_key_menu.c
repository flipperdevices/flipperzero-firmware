#include "../picopass_i.h"
#include "../picopass_keys.h"

enum SubmenuIndex {
    SubmenuIndexWriteStandard,
    SubmenuIndexWriteiCE,
    SubmenuIndexWriteiCL,
    SubmenuIndexWriteiCS,
    SubmenuIndexWriteCustom,
};

void picopass_scene_key_menu_submenu_callback(void* context, uint32_t index) {
    Picopass* picopass = context;

    view_dispatcher_send_custom_event(picopass->view_dispatcher, index);
}

void picopass_scene_key_menu_on_enter(void* context) {
    Picopass* picopass = context;
    Submenu* submenu = picopass->submenu;

    submenu_add_item(
        submenu,
        "Write Standard",
        SubmenuIndexWriteStandard,
        picopass_scene_key_menu_submenu_callback,
        picopass);
    submenu_add_item(
        submenu,
        "Write iCE",
        SubmenuIndexWriteiCE,
        picopass_scene_key_menu_submenu_callback,
        picopass);
    submenu_add_item(
        submenu,
        "Write iCL",
        SubmenuIndexWriteiCL,
        picopass_scene_key_menu_submenu_callback,
        picopass);
    submenu_add_item(
        submenu,
        "Write iCS",
        SubmenuIndexWriteiCS,
        picopass_scene_key_menu_submenu_callback,
        picopass);
    submenu_add_item(
        submenu,
        "Write Elite",
        SubmenuIndexWriteCustom,
        picopass_scene_key_menu_submenu_callback,
        picopass);

    submenu_set_selected_item(
        picopass->submenu,
        scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneKeyMenu));

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewMenu);
}

bool picopass_scene_key_menu_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexWriteStandard) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteStandard);
            memcpy(picopass->dev->dev_data.pacs.key, picopass_iclass_key, RFAL_PICOPASS_BLOCK_LEN);
            picopass->dev->dev_data.pacs.elite_kdf = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCE) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCE);
            memcpy(picopass->dev->dev_data.pacs.key, picopass_xice_key, RFAL_PICOPASS_BLOCK_LEN);
            picopass->dev->dev_data.pacs.elite_kdf = true;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCL) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCL);
            memcpy(picopass->dev->dev_data.pacs.key, picopass_xicl_key, RFAL_PICOPASS_BLOCK_LEN);
            picopass->dev->dev_data.pacs.elite_kdf = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCS) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCS);
            memcpy(picopass->dev->dev_data.pacs.key, picopass_xics_key, RFAL_PICOPASS_BLOCK_LEN);
            picopass->dev->dev_data.pacs.elite_kdf = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteCustom) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteCustom);
            // Key and elite_kdf = true are both set in key_input scene
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneKeyInput);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            picopass->scene_manager, PicopassSceneStart);
    }

    return consumed;
}

void picopass_scene_key_menu_on_exit(void* context) {
    Picopass* picopass = context;

    submenu_reset(picopass->submenu);
}
