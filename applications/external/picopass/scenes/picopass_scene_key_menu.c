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
            memcpy(
                picopass->write_key_context.key_to_write, picopass_iclass_key, PICOPASS_KEY_LEN);
            picopass->write_key_context.is_elite = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCE) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCE);
            memcpy(picopass->write_key_context.key_to_write, picopass_xice_key, PICOPASS_KEY_LEN);
            picopass->write_key_context.is_elite = true;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCL) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCL);
            memcpy(picopass->write_key_context.key_to_write, picopass_xicl_key, PICOPASS_KEY_LEN);
            picopass->write_key_context.is_elite = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteiCS) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteiCS);
            memcpy(picopass->write_key_context.key_to_write, picopass_xics_key, PICOPASS_KEY_LEN);
            picopass->write_key_context.is_elite = false;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteCustom) {
            // If user dictionary, prepopulate with the first key
            if(iclass_elite_dict_check_presence(IclassEliteDictTypeUser)) {
                IclassEliteDict* dict = iclass_elite_dict_alloc(IclassEliteDictTypeUser);
                iclass_elite_dict_get_next_key(dict, picopass->byte_input_store);
                iclass_elite_dict_free(dict);
            }

            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneKeyMenu, SubmenuIndexWriteCustom);
            // Key and elite_kdf = true are both set in key_input scene after the value is input
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
