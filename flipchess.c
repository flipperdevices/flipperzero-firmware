#include "flipchess.h"
#include "helpers/flipchess_haptic.h"

bool flipchess_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipChess* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipchess_tick_event_callback(void* context) {
    furi_assert(context);
    FlipChess* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool flipchess_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipChess* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void text_input_callback(void* context) {
    furi_assert(context);
    FlipChess* app = context;
    bool handled = false;

    // check that there is text in the input
    if(strlen(app->input_text) > 0) {
        if(app->input_state == FlipChessTextInputPassphrase) {
            if(app->passphrase == FlipChessPassphraseOn) {
                strcpy(app->passphrase_text, app->input_text);
            }
            // clear input text
            memzero(app->input_text, TEXT_BUFFER_SIZE);
            // reset input state
            app->input_state = FlipChessTextInputDefault;
            handled = true;
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdSettings);
        } else if(app->input_state == FlipChessTextInputMnemonic) {
            if(app->import_from_mnemonic == 1) {
                strcpy(app->import_mnemonic_text, app->input_text);

                int status = FlipChessStatusSuccess;
                // Check if the mnemonic is valid
                if(mnemonic_check(app->import_mnemonic_text) == 0)
                    status = FlipChessStatusMnemonicCheckError; // 13 = mnemonic check error
                // Save the mnemonic to persistent storage
                else if(!flipchess_save_file_secure(app->import_mnemonic_text))
                    status = FlipChessStatusSaveError; // 12 = save error

                if(status == FlipChessStatusSuccess) {
                    //notification_message(app->notification, &sequence_blink_cyan_100);
                    flipchess_play_happy_bump(app);
                } else {
                    //notification_message(app->notification, &sequence_blink_red_100);
                    flipchess_play_long_bump(app);
                }

                memzero(app->import_mnemonic_text, TEXT_BUFFER_SIZE);
            }
            // clear input text
            memzero(app->input_text, TEXT_BUFFER_SIZE);
            // reset input state
            app->input_state = FlipChessTextInputDefault;
            handled = true;
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdMenu);
        }
    }

    if(!handled) {
        // clear input text
        memzero(app->input_text, TEXT_BUFFER_SIZE);
        // reset input state
        app->input_state = FlipChessTextInputDefault;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdMenu);
    }
}

FlipChess* flipchess_app_alloc() {
    FlipChess* app = malloc(sizeof(FlipChess));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&flipchess_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipchess_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipchess_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, flipchess_custom_event_callback);
    app->submenu = submenu_alloc();

    // Settings
    app->haptic = FlipChessHapticOn;
    app->led = FlipChessLedOn;
    app->bip39_strength = FlipChessStrength256; // 256 bits (24 words)
    app->passphrase = FlipChessPassphraseOff;

    // Main menu
    app->bip44_coin = FlipChessCoinBTC0; // 0 (BTC)
    app->overwrite_saved_seed = 0;
    app->import_from_mnemonic = 0;

    // Text input
    app->input_state = FlipChessTextInputDefault;

    view_dispatcher_add_view(
        app->view_dispatcher, FlipChessViewIdMenu, submenu_get_view(app->submenu));
    app->flipchess_startscreen = flipchess_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipChessViewIdStartscreen,
        flipchess_startscreen_get_view(app->flipchess_startscreen));
    app->flipchess_scene_1 = flipchess_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipChessViewIdScene1, flipchess_scene_1_get_view(app->flipchess_scene_1));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipChessViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->text_input = text_input_alloc();
    text_input_set_result_callback(
        app->text_input,
        text_input_callback,
        (void*)app,
        app->input_text,
        TEXT_BUFFER_SIZE,
        //clear default text
        true);
    text_input_set_header_text(app->text_input, "Input");
    view_dispatcher_add_view(
        app->view_dispatcher, FlipChessViewIdTextInput, text_input_get_view(app->text_input));

    //End Scene Additions

    return app;
}

void flipchess_app_free(FlipChess* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    text_input_free(app->text_input);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipChessViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipChessViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, FlipChessViewIdSettings);
    view_dispatcher_remove_view(app->view_dispatcher, FlipChessViewIdTextInput);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    memzero(app, sizeof(FlipChess));
    free(app);
}

int32_t flipchess_app(void* p) {
    UNUSED(p);
    FlipChess* app = flipchess_app_alloc();

    // Disabled because causes exit on custom firmwares such as RM
    /*if(!furi_hal_region_is_provisioned()) {
        flipchess_app_free(app);
        return 1;
    }*/

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, FlipChessSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, FlipChessSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    flipchess_app_free(app);

    return 0;
}
