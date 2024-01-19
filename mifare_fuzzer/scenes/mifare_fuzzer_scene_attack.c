#include "../mifare_fuzzer_i.h"
#include "../mifare_fuzzer_custom_events.h"

enum SubmenuIndex {
    SubmenuIndexTestValue,
    SubmenuIndexRandomValuesAttack,
    SubmenuIndexLoadUIDsFromFile,
};

/// @brief mifare_fuzzer_scene_attack_submenu_callback()
/// @param context
/// @param index
void mifare_fuzzer_scene_attack_submenu_callback(void* context, uint32_t index) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_submenu_callback() :: index = %ld", index);
    MifareFuzzerApp* app = context;
    uint8_t custom_event = 255;
    switch(index) {
    case SubmenuIndexTestValue:
        custom_event = MifareFuzzerEventTestValueAttack;
        break;
    case SubmenuIndexRandomValuesAttack:
        custom_event = MifareFuzzerEventRandomValuesAttack;
        break;
    case SubmenuIndexLoadUIDsFromFile:
        custom_event = MifareFuzzerEventLoadUIDsFromFileAttack;
        break;
    default:
        return;
    }
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_submenu_callback() :: custom_event = %d", custom_event);
    view_dispatcher_send_custom_event(app->view_dispatcher, custom_event);
}

/// @brief mifare_fuzzer_scene_attack_on_enter()
/// @param context
void mifare_fuzzer_scene_attack_on_enter(void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_on_enter()");
    MifareFuzzerApp* app = context;

    Submenu* submenu_attack = app->submenu_attack;
    submenu_set_header(submenu_attack, "Mifare Fuzzer (attack)");
    submenu_add_item(
        submenu_attack,
        "Test Values",
        SubmenuIndexTestValue,
        mifare_fuzzer_scene_attack_submenu_callback,
        app);
    submenu_add_item(
        submenu_attack,
        "Random Values",
        SubmenuIndexRandomValuesAttack,
        mifare_fuzzer_scene_attack_submenu_callback,
        app);
    submenu_add_item(
        submenu_attack,
        "Load UIDs from file",
        SubmenuIndexLoadUIDsFromFile,
        mifare_fuzzer_scene_attack_submenu_callback,
        app);

    // set selected menu
    submenu_set_selected_item(
        submenu_attack,
        scene_manager_get_scene_state(app->scene_manager, MifareFuzzerSceneAttack));

    view_dispatcher_switch_to_view(app->view_dispatcher, MifareFuzzerViewSelectAttack);
}

/// @brief mifare_fuzzer_scene_attack_on_event()
/// @param context
/// @param event
/// @return
bool mifare_fuzzer_scene_attack_on_event(void* context, SceneManagerEvent event) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_on_event()");
    MifareFuzzerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_on_event() :: event.event = %ld", event.event);
        if(event.event == MifareFuzzerEventTestValueAttack) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneAttack, SubmenuIndexTestValue);
            // set emulator attack
            app->attack = MifareFuzzerAttackTestValues;
            mifare_fuzzer_emulator_set_attack(app->emulator_view, app->attack);
            // open next scene
            scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneEmulator);
            consumed = true;
        } else if(event.event == MifareFuzzerEventRandomValuesAttack) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneAttack, SubmenuIndexRandomValuesAttack);
            // set emulator attack
            app->attack = MifareFuzzerAttackRandomValues;
            mifare_fuzzer_emulator_set_attack(app->emulator_view, app->attack);
            // open next scene
            scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneEmulator);
            consumed = true;
        } else if(event.event == MifareFuzzerEventLoadUIDsFromFileAttack) {
            // save selected item
            scene_manager_set_scene_state(
                app->scene_manager, MifareFuzzerSceneAttack, SubmenuIndexLoadUIDsFromFile);
            // set emulator attack
            app->attack = MifareFuzzerAttackLoadUidsFromFile;
            mifare_fuzzer_emulator_set_attack(app->emulator_view, app->attack);
            // open dialog file
            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(
                &browser_options, MIFARE_FUZZER_UID_FILE_EXT, NULL);
            browser_options.hide_ext = false;
            bool res = dialog_file_browser_show(
                app->dialogs, app->uid_file_path, app->app_folder, &browser_options);
            if(res) {
                app->uids_stream = buffered_file_stream_alloc(app->storage);
                res = buffered_file_stream_open(
                    app->uids_stream,
                    furi_string_get_cstr(app->uid_file_path),
                    FSAM_READ,
                    FSOM_OPEN_EXISTING);
                if(res) {
                    // open next scene
                    scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneEmulator);
                } else {
                    buffered_file_stream_close(app->uids_stream);
                }
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_on_event() :: SceneManagerEventTypeTick");
        //consumed = true;
    }

    return consumed;
}

/// @brief mifare_fuzzer_scene_attack_on_exit()
/// @param context
void mifare_fuzzer_scene_attack_on_exit(void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_scene_attack_on_exit()");
    MifareFuzzerApp* app = context;
    submenu_reset(app->submenu_attack);
}
