#include "flipbip.h"
#include "helpers/flipbip_file.h"
// From: lib/crypto
#include <memzero.h>
#include <bip39.h>

#define MNEMONIC_MENU_DEFAULT "Import mnemonic seed"
#define MNEMONIC_MENU_SUCCESS "Import seed (success)"
#define MNEMONIC_MENU_FAILURE "Import seed (failed!)"

bool flipbip_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipBip* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipbip_tick_event_callback(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool flipbip_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void text_input_callback(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    bool handled = false;

    // check that there is text in the input
    if(strlen(app->input_text) > 0) {
        if(app->input_state == FlipBipTextInputPassphrase) {
            if(app->passphrase == FlipBipPassphraseOn) {
                strcpy(app->passphrase_text, app->input_text);
            }
            // clear input text
            memzero(app->input_text, TEXT_BUFFER_SIZE);
            // reset input state
            app->input_state = FlipBipTextInputDefault;
            handled = true;
            // switch back to settings view
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdSettings);
        } else if(app->input_state == FlipBipTextInputMnemonic) {
            if(app->import_from_mnemonic == 1) {
                strcpy(app->import_mnemonic_text, app->input_text);

                int status = FlipBipStatusSuccess;
                // Check if the mnemonic is valid
                if(mnemonic_check(app->import_mnemonic_text) == 0)
                    status = FlipBipStatusMnemonicCheckError; // 13 = mnemonic check error
                // Save the mnemonic to persistent storage
                else if(!flipbip_save_file_secure(app->import_mnemonic_text))
                    status = FlipBipStatusSaveError; // 12 = save error

                if(status == FlipBipStatusSuccess) {
                    app->mnemonic_menu_text = MNEMONIC_MENU_SUCCESS;
                    //notification_message(app->notification, &sequence_blink_cyan_100);
                    //flipbip_play_happy_bump(app);
                } else {
                    app->mnemonic_menu_text = MNEMONIC_MENU_FAILURE;
                    //notification_message(app->notification, &sequence_blink_red_100);
                    //flipbip_play_long_bump(app);
                }

                memzero(app->import_mnemonic_text, TEXT_BUFFER_SIZE);
            }
            // clear input text
            memzero(app->input_text, TEXT_BUFFER_SIZE);
            // reset input state
            app->input_state = FlipBipTextInputDefault;
            handled = true;
            // exit scene 1 instance that's being used for text input and go back to menu
            scene_manager_previous_scene(app->scene_manager);
            //view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdMenu);
        }
    }

    if(!handled) {
        // clear input text
        memzero(app->input_text, TEXT_BUFFER_SIZE);
        // reset input state
        app->input_state = FlipBipTextInputDefault;
        // something went wrong, switch to menu view
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdMenu);
    }
}

FlipBip* flipbip_app_alloc() {
    FlipBip* app = malloc(sizeof(FlipBip));
    app->gui = furi_record_open(RECORD_GUI);
    //app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on, believe me this makes testing your app easier
    //notification_message(app->notification, &sequence_display_backlight_on);

    // Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&flipbip_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipbip_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipbip_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, flipbip_custom_event_callback);
    app->submenu = submenu_alloc();

    // Settings
    app->bip39_strength = FlipBipStrength256; // 256 bits (24 words)
    app->passphrase = FlipBipPassphraseOff;

    // Main menu
    app->bip44_coin = FlipBipCoinBTC0; // 0 (BTC)
    app->overwrite_saved_seed = 0;
    app->import_from_mnemonic = 0;
    app->mnemonic_menu_text = MNEMONIC_MENU_DEFAULT;

    // Text input
    app->input_state = FlipBipTextInputDefault;

    view_dispatcher_add_view(
        app->view_dispatcher, FlipBipViewIdMenu, submenu_get_view(app->submenu));
    app->flipbip_scene_1 = flipbip_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipBipViewIdScene1, flipbip_scene_1_get_view(app->flipbip_scene_1));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipBipViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->text_input = text_input_alloc();
    text_input_set_result_callback(
        app->text_input,
        text_input_callback,
        (void*)app,
        app->input_text,
        TEXT_BUFFER_SIZE,
        // clear default text
        true);
    //text_input_set_header_text(app->text_input, "Input");
    view_dispatcher_add_view(
        app->view_dispatcher, FlipBipViewIdTextInput, text_input_get_view(app->text_input));

    // End Scene Additions

    return app;
}

void flipbip_app_free(FlipBip* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    text_input_free(app->text_input);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdSettings);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdTextInput);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    //app->notification = NULL;

    //Remove whatever is left
    memzero(app, sizeof(FlipBip));
    free(app);
}

int32_t flipbip_app(void* p) {
    UNUSED(p);
    FlipBip* app = flipbip_app_alloc();

    // Disabled because causes exit on custom firmwares such as RM
    /*if(!furi_hal_region_is_provisioned()) {
        flipbip_app_free(app);
        return 1;
    }*/

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, FlipBipSceneMenu); //Start with menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    flipbip_app_free(app);

    return 0;
}
