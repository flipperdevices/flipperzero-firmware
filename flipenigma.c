#include "flipenigma.h"
#include "helpers/flipenigma_haptic.h"

#define ENIGMA_IMPLEMENTATION
#include "enigma/enigma.h"

bool flipenigma_custom_event_callback(void *context, uint32_t event)
{
    furi_assert(context);
    FlipEnigma *app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipenigma_tick_event_callback(void *context)
{
    furi_assert(context);
    FlipEnigma *app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

// leave app if back button pressed
bool flipenigma_navigation_event_callback(void *context)
{
    furi_assert(context);
    FlipEnigma *app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void text_string_to_uppercase(char *input)
{
    int i;
    for (i = 0; input[i] != '\0'; i++)
    {
        if (input[i] >= 'a' && input[i] <= 'z')
        {
            input[i] = input[i] - 32;
        }
        else
        {
            input[i] = input[i];
        }
    }
}

static void text_build_output(char *input, char *output)
{
    Enigma *e = init_enigma (
			   // rotors model
			   (const char *[]){"M3-II", "M3-I", "M3-III"},
			   // rotor_positions
			   (const uint8_t [ROTORS_N]) {0, 0, 0},
			   // rotor_ring_settings
			   (const uint8_t [ROTORS_N]) {0, 0, 0},
			   // reflector model
			   "M3-B",
			   // plugboard switches
			   (uint8_t [][2]){                      
			     {'A', 'M'}, {'F', 'I'}, {'N', 'V'},
			     {'P', 'S'}, {'T', 'U'}, {'W', 'Z'}},
			   // plugboard size
			   6                                      
			   );

    int out = 0;
    int in;
    for (in = 0; input[in] != '\0'; in++)
    {
        if (input[in] >= 'A' && input[in] <= 'Z')
        {
            char plaintext[2];
            plaintext[0] = input[in];
            plaintext[1] = '\0';
            char ciphertext[2];
            ciphertext[0] = input[in];
            ciphertext[1] = '\0';

            enigma_encrypt(e, plaintext, 1, ciphertext);

            output[out] = ciphertext[0];
        }
        else
        {
            output[out] = input[in]; // do not
        }
        out++;
    }
    //output[out] = '\n';
    //out++;
    output[out] = '\0';

    destroy_enigma(e);
}

static void text_input_callback(void *context)
{
    furi_assert(context);
    FlipEnigma *app = context;
    bool handled = false;

    // check that there is text in the input
    if (strlen(app->input_text) > 0)
    {

        FURI_LOG_D("enigma", "Input text: %s", app->input_text);
        // this is where we build the output.
        text_string_to_uppercase(app->input_text);
        FURI_LOG_D("enigma", "Upper text: %s", app->input_text);
        // this does the actual work of encrypting the text
        text_build_output(app->input_text, app->cipher_text);
        
        text_box_set_text(app->text_box, app->cipher_text);
        
        // reset input state
        app->input_state = FlipEnigmaTextInputDefault;
        handled = true;

        // TODO
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdTextBox);
    }

    if (!handled)
    {
        // reset input state
        app->input_state = FlipEnigmaTextInputDefault;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdMenu);
    }
}

FlipEnigma *flipenigma_app_alloc()
{
    FlipEnigma *app = malloc(sizeof(FlipEnigma));
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
    app->white_mode = FlipEnigmaPlayerHuman;
    app->black_mode = FlipEnigmaPlayerAI1;

    // Startscreen
    app->sound = 0;
    // Main menu
    app->import_game = 0;

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

    app->text_input = text_input_alloc();
    text_input_set_result_callback(
        app->text_input,
        text_input_callback,
        (void *)app,
        app->input_text,
        TEXT_BUFFER_SIZE,
        // clear default text
        true);
    text_input_set_header_text(app->text_input, "Input");
    view_dispatcher_add_view(
        app->view_dispatcher, FlipEnigmaViewIdTextInput, text_input_get_view(app->text_input));
    view_dispatcher_add_view(
        app->view_dispatcher, FlipEnigmaViewIdTextBox, text_box_get_view(app->text_box));

    // End Scene Additions

    return app;
}

void flipenigma_app_free(FlipEnigma *app)
{
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
    view_dispatcher_remove_view(app->view_dispatcher, FlipEnigmaViewIdTextInput);
    text_input_free(app->text_input);
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

int32_t flipenigma_app(void *p)
{
    UNUSED(p);
    FlipEnigma *app = flipenigma_app_alloc();

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
