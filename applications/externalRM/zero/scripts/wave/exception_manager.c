#include "exception_manager.h"
#include "../app.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const int EXCEPTION_SCENE_ID = -10000;
const char* exception_message = NULL;

void throw_exception(const char* message) {
    FURI_LOG_E("EXCEPTION_MANAGER", "Exception: %s", message);
    exception_message = message;
}

void throw_exception_if(bool condition, const char* message) {
    if(condition) throw_exception(message);
}

bool exception_manager_has_exception() {
    return exception_message != NULL;
}

const char* get_exception_message() {
    return exception_message;
}

void exception_manager_render_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_draw_str_aligned(canvas, 63, 32, AlignCenter, AlignCenter, exception_message);
}

void exception_manager_input_callback(InputKey key, InputType type, void* context) {
    SceneManager* sceneManager = (SceneManager*)context;

    if(key == InputKeyBack && type == InputTypePress) {
        scene_manager_set_scene(sceneManager, SCENE_MANAGER_NO_SCENE);
        return;
    }
}

void exception_manager_handle_exceptions(SceneManager* sceneManager) {
    if(!exception_manager_has_exception() ||
       scene_manager_get_current_scene_id(sceneManager) == EXCEPTION_SCENE_ID)
        return;

    Scene* exceptionScene = scene_alloc(
        exception_manager_render_callback,
        NULL,
        exception_manager_input_callback,
        NULL,
        sceneManager);
    scene_manager_register_scene(sceneManager, EXCEPTION_SCENE_ID, exceptionScene);
    scene_manager_set_scene(sceneManager, EXCEPTION_SCENE_ID);
}