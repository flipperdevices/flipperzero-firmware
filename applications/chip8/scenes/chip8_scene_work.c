#include "../chip8_app_i.h"
#include "../views/chip8_view.h"
#include "furi_hal.h"

void chip8_scene_ok_callback(InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

void chip8_scene_back_callback(Chip8View* view, InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    chip8_set_back_pressed(app->chip8);
    chip8_set_state(view, chip8_get_state(app->chip8));
//    view_dispatcher_send_custom_event(app->view_dispatcher, type);
    view_dispatcher_stop(app->view_dispatcher);
}

void chip8_scene_up_callback(InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    chip8_set_up_pressed(app->chip8);
//    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

void chip8_scene_down_callback(InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    chip8_set_down_pressed(app->chip8);
//    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

void chip8_scene_release_callback(InputType type, void* context) {
    furi_assert(context);
    Chip8App* app = context;
    chip8_release_keyboard(app->chip8);
    //    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

bool chip8_scene_work_on_event(void* context, SceneManagerEvent event) {
    Chip8App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        chip8_toggle(app->chip8);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        Chip8State* state = chip8_get_state(app->chip8);

        chip8_set_state(app->chip8_view, state);
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

    FURI_LOG_I("chip8_scene_work_on_enter", "START SET BACKUP SCREEN");
    chip8_set_backup_screen(app->chip8_view, app->backup_screen);
    FURI_LOG_I("chip8_scene_work_on_enter", "END SET BACKUP SCREEN");

    app->chip8 = chip8_make_emulator(file_name);

    string_clear(file_name);

    chip8_set_state(app->chip8_view, chip8_get_state(app->chip8));

    chip8_set_ok_callback(app->chip8_view, chip8_scene_ok_callback, app);
    chip8_set_back_callback(app->chip8_view, chip8_scene_back_callback, app);
    chip8_set_up_callback(app->chip8_view, chip8_scene_up_callback, app);
    chip8_set_down_callback(app->chip8_view, chip8_scene_down_callback, app);
    chip8_set_release_callback(app->chip8_view, chip8_scene_release_callback, app);


    view_dispatcher_switch_to_view(app->view_dispatcher, Chip8WorkView);
}

void chip8_scene_work_on_exit(void* context) {
    Chip8App* app = context;
    chip8_close_emulator(app->chip8);
}
