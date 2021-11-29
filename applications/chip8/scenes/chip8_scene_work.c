#include "../chip8_app_i.h"
#include "../views/chip8_view.h"
#include "furi-hal.h"

void chip8_scene_work_ok_callback(InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

bool chip8_scene_work_on_event(void* context, SceneManagerEvent event) {
    //Chip8App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //bad_usb_script_toggle(app->bad_usb_script);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        //bad_usb_set_state(app->bad_usb_view, bad_usb_script_get_state(app->bad_usb_script));
    }
    return consumed;
}

void chip8_scene_work_on_enter(void* context) {
    Chip8App* app = context;

    string_t file_name;
    string_init(file_name);

    chip8_set_file_name(app->chip8_view, app->file_name);
    string_printf(
        file_name, "%s/%s%s", CHIP8_APP_PATH_FOLDER, app->file_name, CHIP8_APP_EXTENSION);

    app->chip8 = chip8_make_emulator(file_name);

    string_clear(file_name);

    chip8_set_state(app->chip8_view, chip8_get_state(app->chip8));

    chip8_set_ok_callback(app->chip8_view, chip8_scene_work_ok_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, Chip8WorkView);
}

void chip8_scene_work_on_exit(void* context) {
    Chip8App* app = context;
    chip8_close_emulator(app->chip8);
}
