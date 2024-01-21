#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_add_code.h"
#include <furi_hal_cortex.h>

void add_code_text_input_callback(void* context) {
    App* app = context;
    FURI_LOG_I(TAG, "save_text_input_callback %s", app->state->file_name_tmp);
    //view_dispatcher_send_custom_event(subghz->view_dispatcher, SubGhzCustomEventSceneSaveName);
    scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
}

void fcom_add_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Enter Code");
    text_input_set_result_callback(app->text_input,
                                   add_code_text_input_callback,
                                   app,
                                   app->state->file_name_tmp,
                                   MAX_FILENAME_LEN,
                                   true);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomKeyboardView);
}

bool fcom_add_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    return false; //consumed event
}

void fcom_add_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
}


