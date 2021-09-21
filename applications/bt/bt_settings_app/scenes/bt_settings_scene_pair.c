#include "../bt_settings_app.h"
#include "furi-hal-bt.h"

#define BT_SETTINGS_SCENE_PAIR_BACK_EVENT (0UL)
#define BT_SETTINGS_SCENE_PAIR_COMPLETE_EVENT (1UL)

static void bt_settings_scene_pair_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    BtSettingsApp* app = context;
    if(result == DialogExResultLeft) {
        view_dispatcher_send_custom_event(app->view_dispatcher, BT_SETTINGS_SCENE_PAIR_BACK_EVENT);
    }
}

void bt_settings_scene_pair_on_enter(void* context) {
    BtSettingsApp* app = context;
    if(app->settings.enabled) {
        widget_add_string_multiline_element(
            app->widget,
            70,
            9,
            AlignLeft,
            AlignTop,
            FontSecondary,
            " Waiting for\na Bluetooth\n     friend ...");
        widget_add_icon_element(app->widget, 87, 43, &I_BLE_big_11x17);
        widget_add_icon_element(app->widget, 0, 5, &I_DolphinWait_61x59);
        view_dispatcher_switch_to_view(app->view_dispatcher, BtSettingsAppViewWidget);
        // Call advertise
    } else {
        dialog_ex_set_context(app->dialog, app);
        dialog_ex_set_header(app->dialog, "Activate\nBluetooth", 10, 32, AlignLeft, AlignCenter);
        dialog_ex_set_icon(app->dialog, 70, 22, &I_BLE_big_11x17);
        dialog_ex_set_result_callback(app->dialog, bt_settings_scene_pair_dialog_callback);
        dialog_ex_set_left_button_text(app->dialog, "Back");
        view_dispatcher_switch_to_view(app->view_dispatcher, BtSettingsAppViewDialog);
    }
}

bool bt_settings_scene_pair_on_event(void* context, SceneManagerEvent event) {
    BtSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BT_SETTINGS_SCENE_PAIR_BACK_EVENT) {
            scene_manager_previous_scene(app->scene_manager);
        } else if(event.event == BT_SETTINGS_SCENE_PAIR_COMPLETE_EVENT) {
            // scene_manager_next_scene()
        }
        consumed = true;
    }
    return consumed;
}

void bt_settings_scene_pair_on_exit(void* context) {
    BtSettingsApp* app = context;
    widget_clear(app->widget);
    dialog_ex_clean(app->dialog);
}
