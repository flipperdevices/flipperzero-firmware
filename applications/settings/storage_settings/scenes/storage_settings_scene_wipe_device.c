#include "../storage_settings.h"
#include <furi_hal.h>

#define STORAGE_SETTINGS_SCENE_WIPE_DEVICE_CONFIRM_COUNT 5

static void
    storage_settings_scene_wipe_device_dialog_callback(DialogExResult result, void* context) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void storage_settings_scene_wipe_device_on_enter(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(dialog_ex, storage_settings_scene_wipe_device_dialog_callback);

    dialog_ex_set_left_button_text(dialog_ex, "Cancel");
    dialog_ex_set_right_button_text(dialog_ex, "Erase");

    dialog_ex_set_header(dialog_ex, "Confirm full Wipe", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(
        dialog_ex,
        "Everything will be erased\r\nData and settings will be lost!",
        64,
        32,
        AlignCenter,
        AlignCenter);

    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewDialogEx);
}

bool storage_settings_scene_wipe_device_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    uint32_t counter =
        scene_manager_get_scene_state(app->scene_manager, StorageSettingsWipeDevice);

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultLeft:
            scene_manager_set_scene_state(app->scene_manager, StorageSettingsWipeDevice, 0);
            consumed = scene_manager_previous_scene(app->scene_manager);
            if(!consumed) {
                view_dispatcher_stop(app->view_dispatcher);
            }
            break;
        case DialogExResultRight:
            counter++;
            if(counter < STORAGE_SETTINGS_SCENE_WIPE_DEVICE_CONFIRM_COUNT) {
                furi_string_printf(
                    app->text_string,
                    "%ld presses left",
                    STORAGE_SETTINGS_SCENE_WIPE_DEVICE_CONFIRM_COUNT - counter);
                dialog_ex_set_text(
                    app->dialog_ex,
                    furi_string_get_cstr(app->text_string),
                    64,
                    32,
                    AlignCenter,
                    AlignCenter);
                scene_manager_set_scene_state(
                    app->scene_manager, StorageSettingsWipeDevice, counter);
            } else {
                furi_hal_rtc_set_flag(FuriHalRtcFlagFactoryReset);
                scene_manager_set_scene_state(app->scene_manager, StorageSettingsFormatting, true);
                scene_manager_next_scene(app->scene_manager, StorageSettingsFormatting);
            }

            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void storage_settings_scene_wipe_device_on_exit(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);

    furi_string_reset(app->text_string);
}