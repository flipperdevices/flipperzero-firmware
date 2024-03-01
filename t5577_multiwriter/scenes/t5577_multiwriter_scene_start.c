#include "../t5577_multiwriter_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    SubmenuIndexWriteFirstKey,
    SubmenuIndexWriteSecondKey,
    SubmenuIndexWriteThirdKey,
} SubmenuIndex;

static void t5577_multiwriter_scene_start_submenu_callback(void* context, uint32_t index) {
    LfRfid* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void t5577_multiwriter_scene_start_on_enter(void* context) {
    LfRfid* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Write first key",
        SubmenuIndexWriteFirstKey,
        t5577_multiwriter_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Write second key",
        SubmenuIndexWriteSecondKey,
        t5577_multiwriter_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Write third key",
        SubmenuIndexWriteThirdKey,
        t5577_multiwriter_scene_start_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, LfRfidSceneStart));

    // clear key
    furi_string_reset(app->file_name);
    app->protocol_id = PROTOCOL_NO;
    app->read_type = LFRFIDWorkerReadTypeAuto;

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewSubmenu);
}

bool t5577_multiwriter_scene_start_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexWriteFirstKey) {
            scene_manager_set_scene_state(
                app->scene_manager, LfRfidSceneStart, SubmenuIndexWriteFirstKey);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectFirstKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteSecondKey) {
            scene_manager_set_scene_state(
                app->scene_manager, LfRfidSceneStart, SubmenuIndexWriteSecondKey);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectSecondKey);
            consumed = true;
        } else if(event.event == SubmenuIndexWriteThirdKey) {
            scene_manager_set_scene_state(
                app->scene_manager, LfRfidSceneStart, SubmenuIndexWriteThirdKey);
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectThirdKey);
            consumed = true;
        }
    }

    return consumed;
}

void t5577_multiwriter_scene_start_on_exit(void* context) {
    LfRfid* app = context;

    submenu_reset(app->submenu);
}
