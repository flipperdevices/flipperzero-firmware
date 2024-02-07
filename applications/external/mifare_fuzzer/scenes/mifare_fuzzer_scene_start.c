#include "../mifare_fuzzer_i.h"
#include "../mifare_fuzzer_custom_events.h"

enum SubmenuIndex {
    SubmenuIndexClassic1k,
    SubmenuIndexClassic4k,
    SubmenuIndexUltralight,
    SubmenuIndexFile,
};

/// @brief mifare_fuzzer_scene_start_submenu_callback()
/// @param context
/// @param index
void mifare_fuzzer_scene_start_submenu_callback(void* context, uint32_t index) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_submenu_callback() :: index = %ld", index);
    MifareFuzzerApp* app = context;
    uint8_t custom_event = 255;
    switch(index) {
    case SubmenuIndexClassic1k:
        custom_event = MifareFuzzerEventClassic1k;
        break;
    case SubmenuIndexClassic4k:
        custom_event = MifareFuzzerEventClassic4k;
        break;
    case SubmenuIndexUltralight:
        custom_event = MifareFuzzerEventUltralight;
        break;
    case SubmenuIndexFile:
        custom_event = MifareFuzzerEventFile;
        break;
    default:
        return;
    }
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_submenu_callback() :: custom_event = %d", custom_event);
    view_dispatcher_send_custom_event(app->view_dispatcher, custom_event);
}

/// @brief mifare_fuzzer_scene_start_on_enter()
/// @param context
void mifare_fuzzer_scene_start_on_enter(void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_on_enter()");
    MifareFuzzerApp* app = context;

    Submenu* submenu_card = app->submenu_card;
    submenu_set_header(submenu_card, "Mifare Fuzzer (card)");
    submenu_add_item(
        submenu_card,
        "Classic 1k",
        SubmenuIndexClassic1k,
        mifare_fuzzer_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu_card,
        "Classic 4k",
        SubmenuIndexClassic4k,
        mifare_fuzzer_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu_card,
        "Ultralight",
        SubmenuIndexUltralight,
        mifare_fuzzer_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu_card,
        "From file",
        SubmenuIndexFile,
        mifare_fuzzer_scene_start_submenu_callback,
        app);

    // set selected menu
    submenu_set_selected_item(
        submenu_card, scene_manager_get_scene_state(app->scene_manager, MifareFuzzerSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, MifareFuzzerViewSelectCard);
}

/// @brief mifare_fuzzer_scene_start_on_event()
/// @param context
/// @param event
/// @return
/// @param context
/// @param event
/// @return
bool mifare_fuzzer_scene_start_on_event(void* context, SceneManagerEvent event) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_on_event()");
    MifareFuzzerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_on_event() :: event.event = %ld", event.event);
        app->card_file_path = NULL;
        if(event.event == MifareFuzzerEventClassic1k) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneStart, SubmenuIndexClassic1k);
            // set emulator card
            app->card = MifareCardClassic1k;
            mifare_fuzzer_emulator_set_card(app->emulator_view, app->card, NULL);
            // open next scene
            scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneAttack);
            consumed = true;
        } else if(event.event == MifareFuzzerEventClassic4k) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneStart, SubmenuIndexClassic4k);
            // set emulator card
            app->card = MifareCardClassic4k;
            mifare_fuzzer_emulator_set_card(app->emulator_view, app->card, NULL);
            // open next scene
            scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneAttack);
            consumed = true;
        } else if(event.event == MifareFuzzerEventUltralight) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneStart, SubmenuIndexUltralight);
            // set emulator card
            app->card = MifareCardUltralight;
            mifare_fuzzer_emulator_set_card(app->emulator_view, app->card, NULL);
            // open next scene
            scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneAttack);
            consumed = true;
        } else if(event.event == MifareFuzzerEventFile) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneStart, SubmenuIndexFile);
            // Don't set emulator card (yet), read file instead
            DialogsFileBrowserOptions browser_options;
            FuriString* initial_path = furi_string_alloc_set("/ext/nfc");
            dialog_file_browser_set_basic_options(
                &browser_options, MIFARE_FUZZER_CARD_FILE_EXT, NULL);
            browser_options.hide_ext = false;
            app->card_file_path = furi_string_alloc();
            bool wasFileSelected = dialog_file_browser_show(
                app->dialogs, app->card_file_path, initial_path, &browser_options);
            furi_string_free(initial_path);
            if(wasFileSelected) {
                // open next scene
                scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneAttack);
                consumed = true;
            }
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_on_event() :: SceneManagerEventTypeTick");
        //consumed = true;
    }

    return consumed;
}

/// @brief mifare_fuzzer_scene_start_on_exit()
/// @param context
void mifare_fuzzer_scene_start_on_exit(void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_start_on_exit()");
    MifareFuzzerApp* app = context;
    submenu_reset(app->submenu_card);
}
