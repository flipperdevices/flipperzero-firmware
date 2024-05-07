#include "../picopass_i.h"

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexSaveAsLF,
    SubmenuIndexSaveAsSeader,
    SubmenuIndexParse,
    SubmenuIndexChangeKey,
    SubmenuIndexWrite,
    SubmenuIndexEmulate,
    SubmenuIndexSavePartial,
    SubmenuIndexSaveLegacy,
};

void picopass_scene_card_menu_submenu_callback(void* context, uint32_t index) {
    Picopass* picopass = context;

    view_dispatcher_send_custom_event(picopass->view_dispatcher, index);
}

void picopass_scene_card_menu_on_enter(void* context) {
    Picopass* picopass = context;
    Submenu* submenu = picopass->submenu;
    PicopassPacs* pacs = &picopass->dev->dev_data.pacs;
    PicopassBlock* card_data = picopass->dev->dev_data.card_data;
    PicopassDeviceAuthMethod auth = picopass->dev->dev_data.auth;
    PluginWiegand* plugin = picopass->plugin_wiegand;

    bool SE = card_data[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].valid &&
              0x30 == card_data[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data[0];
    bool SR = card_data[PICOPASS_ICLASS_PACS_CFG_BLOCK_INDEX].data[0] == 0xA3 &&
              card_data[10].valid && 0x30 == card_data[10].data[0];
    bool has_sio = SE || SR;
    bool secured = (card_data[PICOPASS_CONFIG_BLOCK_INDEX].data[7] & PICOPASS_FUSE_CRYPT10) !=
                   PICOPASS_FUSE_CRYPT0;
    bool no_credential = picopass_is_memset(pacs->credential, 0x00, sizeof(pacs->credential));

    if(auth == PicopassDeviceAuthMethodFailed) {
        submenu_add_item(
            submenu,
            "Save Partial",
            SubmenuIndexSavePartial,
            picopass_scene_card_menu_submenu_callback,
            picopass);
    } else {
        submenu_add_item(
            submenu, "Save", SubmenuIndexSave, picopass_scene_card_menu_submenu_callback, picopass);
    }

    if(secured && has_sio) {
        submenu_add_item(
            submenu,
            "Save in Seader fmt",
            SubmenuIndexSaveAsSeader,
            picopass_scene_card_menu_submenu_callback,
            picopass);
    }

    if(secured && !no_credential) {
        submenu_add_item(
            submenu,
            "Save as LFRFID",
            SubmenuIndexSaveAsLF,
            picopass_scene_card_menu_submenu_callback,
            picopass);

        if(SR) {
            submenu_add_item(
                submenu,
                "Save as Legacy",
                SubmenuIndexSaveLegacy,
                picopass_scene_card_menu_submenu_callback,
                picopass);
        }

        if(plugin) {
            // Convert from byte array to uint64_t
            uint64_t credential = 0;
            memcpy(&credential, pacs->credential, sizeof(uint64_t));
            credential = __builtin_bswap64(credential);

            size_t format_count = plugin->count(pacs->bitLength, credential);
            if(format_count > 0) {
                submenu_add_item(
                    submenu,
                    "Parse",
                    SubmenuIndexParse,
                    picopass_scene_card_menu_submenu_callback,
                    picopass);
            }
        }
    }

    if(auth == PicopassDeviceAuthMethodNone || auth == PicopassDeviceAuthMethodKey) {
        submenu_add_item(
            submenu,
            "Write",
            SubmenuIndexWrite,
            picopass_scene_card_menu_submenu_callback,
            picopass);
        submenu_add_item(
            submenu,
            "Emulate",
            SubmenuIndexEmulate,
            picopass_scene_card_menu_submenu_callback,
            picopass);
        if(secured) {
            submenu_add_item(
                submenu,
                "Change Key",
                SubmenuIndexChangeKey,
                picopass_scene_card_menu_submenu_callback,
                picopass);
        }
    }

    submenu_set_selected_item(
        picopass->submenu,
        scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneCardMenu));

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewMenu);
}

bool picopass_scene_card_menu_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSave) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexSave);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneSaveName);
            picopass->dev->format = PicopassDeviceSaveFormatOriginal;
            consumed = true;
        } else if(event.event == SubmenuIndexSavePartial) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexSave);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneSaveName);
            picopass->dev->format = PicopassDeviceSaveFormatPartial;
            consumed = true;
        } else if(event.event == SubmenuIndexSaveAsSeader) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, event.event);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneSaveName);
            picopass->dev->format = PicopassDeviceSaveFormatSeader;
            consumed = true;
        } else if(event.event == SubmenuIndexSaveAsLF) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexSaveAsLF);
            picopass->dev->format = PicopassDeviceSaveFormatLF;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteCard);
            consumed = true;
        } else if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexChangeKey) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexChangeKey);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneKeyMenu);
            consumed = true;
        } else if(event.event == SubmenuIndexParse) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexParse);
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneFormats);
            consumed = true;
        } else if(event.event == SubmenuIndexSaveLegacy) {
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneCardMenu, SubmenuIndexSaveLegacy);
            picopass->dev->format = PicopassDeviceSaveFormatLegacy;
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneSaveName);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            picopass->scene_manager, PicopassSceneStart);
    }

    return consumed;
}

void picopass_scene_card_menu_on_exit(void* context) {
    Picopass* picopass = context;

    submenu_reset(picopass->submenu);
}
