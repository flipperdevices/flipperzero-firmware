#include "flipenigma.h"
#include "helpers/flipenigma_haptic.h"
#include "helpers/flipenigma_text.h"

bool flipenigma_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipEnigma* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipenigma_tick_event_callback(void* context) {
    furi_assert(context);
    FlipEnigma* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

// leave app if back button pressed
bool flipenigma_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipEnigma* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

FlipEnigma* flipenigma_app_alloc() {
    FlipEnigma* app = malloc(sizeof(FlipEnigma));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    // Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&flipenigma_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipenigma_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipenigma_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, flipenigma_custom_event_callback);

    // Settings
    app->haptic = FlipEnigmaHapticOn;
    // Settings - Enigma
    app->rotors_model[0] = "M3-III";
    app->rotors_model[1] = "M3-II";
    app->rotors_model[2] = "M3-I";
    app->rotor_positions[0] = FlipEnigma01A;
    app->rotor_positions[1] = FlipEnigma01A;
    app->rotor_positions[2] = FlipEnigma01A;
    app->rotor_ring_settings[0] = FlipEnigma01A;
    app->rotor_ring_settings[1] = FlipEnigma01A;
    app->rotor_ring_settings[2] = FlipEnigma01A;
    app->reflector_model = "M3-B";
    app->plugboard_size = 0;

    // DEBUG
    // app->plugboard_switches[0][0] = 'A';
    // app->plugboard_switches[0][1] = 'M';
    // app->plugboard_switches[1][0] = 'F';
    // app->plugboard_switches[1][1] = 'I';
    // app->plugboard_switches[2][0] = 'N';
    // app->plugboard_switches[2][1] = 'V';
    // app->plugboard_switches[3][0] = 'P';
    // app->plugboard_switches[3][1] = 'S';
    // app->plugboard_switches[4][0] = 'T';
    // app->plugboard_switches[4][1] = 'U';
    // app->plugboard_switches[5][0] = 'W';
    // app->plugboard_switches[5][1] = 'Z';
    // app->plugboard_size = 6;

    // Text input
    app->input_state = FlipEnigmaTextInputDefault;

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipEnigmaViewIdMenu, submenu_get_view(app->submenu));
    app->flipenigma_startscreen = flipenigma_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipEnigmaViewIdStartscreen,
        flipenigma_startscreen_get_view(app->flipenigma_startscreen));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipEnigmaViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->message_input = text_input_alloc();
    text_input_set_result_callback(
        app->message_input,
        text_input_callback,
        (void*)app,
        app->input_message_text,
        TEXT_BUFFER_SIZE,
        // clear default text
        true);
    text_input_set_header_text(app->message_input, "Input message");
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipEnigmaViewIdMessageInput,
        text_input_get_view(app->message_input));

    app->plugboard_input = text_input_alloc();
    text_input_set_result_callback(
        app->plugboard_input,
        text_input_callback,
        (void*)app,
        app->input_plugboard_text,
        TEXT_BUFFER_SIZE,
        // clear default text
        true);
    text_input_set_header_text(app->plugboard_input, "Input plugboard or X to disable");
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipEnigmaViewIdPlugboardInput,
        text_input_get_view(app->plugboard_input));

    app->text_box = text_box_alloc();
    text_box_set_font(app->text_box, TextBoxFontText);
    view_dispatcher_add_view(
        app->view_dispatcher, FlipEnigmaViewIdTextBox, text_box_get_view(app->text_box));

    // End Scene Additions

    return app;
}

void flipenigma_app_free(FlipEnigma* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdMenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdStartscreen);
    flipenigma_startscreen_free(app->flipenigma_startscreen);
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdSettings);
    variable_item_list_free(app->variable_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdMessageInput);
    text_input_free(app->message_input);
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdPlugboardInput);
    text_input_free(app->plugboard_input);
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdTextBox);
    text_box_free(app->text_box);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    // Remove whatever is left
    // memzero(app, sizeof(FlipEnigma));
    free(app);
}

int32_t flipenigma_app(void* p) {
    UNUSED(p);
    FlipEnigma* app = flipenigma_app_alloc();

    // Disabled because causes exit on custom firmwares such as RM
    /*if(!furi_hal_region_is_provisioned()) {
        flipenigma_app_free(app);
        return 1;
    }*/

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, FlipEnigmaSceneStartscreen); // Start with start screen
    // scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneMenu); //if you want to directly start with Menu

    furi_hal_random_init();
    // furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    // furi_hal_power_suppress_charge_exit();
    flipenigma_app_free(app);

    return 0;
}
